//
// Created by Administrator on 2018/12/5/005.
//

#include <my_log.h>
#include "GifMake.h"

GifMake::GifMake(const char *inputPath, const char *outPath, int startSecond, int endSecond) {
    isExit = false;
    frameCount = 0;
    outFormat = AV_PIX_FMT_RGB8;
    int result = buildInput(inputPath);
    if (result < 0) {
        LOGE(" open input faild !");
        return;
    }
    outWidth = vCtxD->width;
    outHeight = vCtxD->height;
    result = buildOutput(outPath);
    if (result < 0) {
        LOGE(" buildOutput faild !");
        return;
    }
    LOGE("out width %d , outheight %d ", outWidth, outHeight);
    result = initSwsContext(vCtxD->width, vCtxD->height, vCtxD->pix_fmt);
    if (result < 0) {
        LOGE(" initSwsContext faild !");
        return;
    }
    this->startSecond = startSecond;
    this->endSecond = endSecond;
}


int GifMake::buildOutput(const char *outPath) {

    int result;
    afc_output = NULL;
    result = initOutput(outPath, &afc_output);
    if (result < 0) {
        LOGE(" initOutput faild !");
        return -1;
    }

    AVCodecParameters *codecpar = avcodec_parameters_alloc();
    codecpar->width = outWidth;
    codecpar->height = outHeight;
    codecpar->format = outFormat;
    result = addOutputVideoStream(afc_output, &vCtxE, *codecpar);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    avcodec_parameters_free(&codecpar);
    videoOutputStreamIndex = result;
    result = writeOutoutHeader(afc_output, outPath);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    vCalDuration = AV_TIME_BASE / outFrameRate;
#ifdef DEBUG
    av_dump_format(afc_output, 0, outPath, 1);
#endif
    LOGE(" VIDEO OUTPUT STREAM %d  ", videoOutputStreamIndex);
    LOGE(" INIT OUTPUT SUCCESS GIF !");
    return 1;
}


int GifMake::buildInput(const char *inputPath) {
    afc_input = NULL;
    int result = open_input_file(inputPath, &afc_input);
    if (result < 0) {
        LOGE(" open_input_file faild  ");
        return -1;
    }
    result = getVideoDecodeContext(afc_input, &vCtxD);
    if (result < 0) {
        LOGE(" getVideoDecodeContext faild  ");
        return -1;
    }
    videoStreamIndex = result;
    return 1;
}

int GifMake::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outWidth, outHeight,
                         outFormat, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {

        return -1;
    }
    return 1;
}

void GifMake::destroySwsContext() {
    if (sws != NULL) {
        sws_freeContext(sws);
        sws = NULL;
    }
}


int GifMake::startParse() {
    int result;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *outVFrame = av_frame_alloc();
    outVFrame->width = outWidth;
    outVFrame->height = outHeight;
    outVFrame->format = outFormat;
    result = av_frame_get_buffer(outVFrame, 0);
    if (result < 0) {
        LOGE(" av_frame_get_buffer FAILD ! %s ", av_err2str(result));
        return -1;
    }
    //这里需要直接seek到裁剪的开始时间，节约时间。然后直接开始解码。就不会存在花屏问题。
    result = av_seek_frame(afc_input, -1,
                           ((float) startSecond / 1000) * AV_TIME_BASE * afc_input->start_time,
                           AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE(" SEEK FRAME FAILD ! %s ", av_err2str(result));
        return -1;
    }
    int64_t pts = 0;
    while (!isExit) {
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            LOGE(" ************* startDecode av_read_frame FAILD ! %s ", av_err2str(result));
            break;
        }
        AVFrame *frame = NULL;
        if (pkt->stream_index == videoStreamIndex) {
            frame = decodePacket(vCtxD, pkt);
            av_packet_unref(pkt);
            if (frame != NULL) {
                pts = (int64_t) (frame->pts *
                                 av_q2d(afc_input->streams[videoStreamIndex]->time_base) * 1000);
                if ((pts >= startSecond * 1000) && (pts <= endSecond * 1000)) {
                    progress = (int) ((float) (pts - startSecond * 1000) / 1000 /
                                      (endSecond - startSecond) * 100);
                    if (progress > 0) {
                        progress--; //100表示完全搞定。可以直接播放
                    }
                    result = av_frame_make_writable(outVFrame);
                    if (result < 0) {
                        LOGE(" av_frame_get_buffer FAILD ! ");
                        av_frame_free(&frame);
                        return -1;
                    }
                    sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                              0, frame->height, outVFrame->data, outVFrame->linesize);
                    outVFrame->pts = frameCount * vCalDuration;
                    frameCount++;

                    AVPacket *vPkt = encodeFrame(outVFrame, vCtxE);
                    if (vPkt != NULL) {
//                        LOGE(" WIRTE GIF FRAME %lld  , size %d " , vPkt->pts , vPkt->size);
                        av_packet_rescale_ts(vPkt, timeBaseFFmpeg,
                                             afc_output->streams[videoOutputStreamIndex]->time_base);
                        vPkt->stream_index = 0;
                        result = av_write_frame(afc_output, vPkt);
                        av_packet_free(&vPkt);
                        if (result < 0) {
                            LOGE(" av_write_frame %s ", av_err2str(result));
                            av_frame_free(&frame);
                            break;
                        }
                    }
                }
                av_frame_free(&frame);
            }
        } else {
            av_packet_unref(pkt);
        }
    }
    av_packet_free(&pkt);
    av_frame_free(&outVFrame);
    progress = 100;
    LOGE(" END ");
    writeTrail(afc_output);
    return 1;
}

GifMake::~GifMake() {
    isExit = true;
    destroySwsContext();
    if (vCtxD != NULL) {
        avcodec_free_context(&vCtxD);
        vCtxD = NULL;
    }
    if (afc_input != NULL) {
        avformat_free_context(afc_input);
        afc_input = NULL;
    }
    if (vCtxE != NULL) {
        avcodec_free_context(&vCtxE);
        vCtxE = NULL;
    }
    if (afc_output != NULL) {
        avformat_free_context(afc_output);
        afc_output = NULL;
    }
}


