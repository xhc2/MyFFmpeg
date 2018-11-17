/**
 * 倒放
 * 视频部分：
 * 找到一个gop然后顺序解码（实践发现是找不到的一个gop有多少帧）
 * 一个解码的gop
 * 然后写入yuv文件中，然后倒序读入中进行编码
 * 音频部分：
 * 直接写入队列中缓存队列中。
 *
 * 输出部分尽量和原文件保持一致（采样率，声道，之类的）
 *
 * seek的操作需要容器支持
 * http://bbs.chinaffmpeg.com/forum.php?mod=viewthread&tid=14
 */

#include <my_log.h>
#include "VideoRunBack.h"

VideoRunBack::VideoRunBack(const char *path, const char *outPath) {

    int inputLen = strlen(path);
    inputLen++;
    this->inputPath = (char *) malloc(inputLen);
    strcpy(this->inputPath, path);

    int len = strlen(outPath);
    len++;
    this->outPath = (char *) malloc(len);
    strcpy(this->outPath, outPath);
    initValue();

}

void VideoRunBack::initValue() {
    afc_output = NULL;
    afc_input = NULL;
    videoCodecD = NULL;
    audioIndexInput = -1;
    videoIndexInput = -1;
    videoIndexOutput = -1;
    audioIndexOutput = -1;
    outFrameRate = 25;
    afot = NULL;
    audioOutStream = NULL;
    videoOutStream = NULL;
    gopFrameCount = 0;
    inputDuration = 0;
    gopCount = 0;
    ffmpegTimeBase = (AVRational) {1, AV_TIME_BASE};
    frameDuration = 0;
}


int VideoRunBack::initInput() {
    int result;
    result = avformat_open_input(&afc_input, inputPath, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return -1;
    }
    result = avformat_find_stream_info(afc_input, 0);
    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return -1;
    }
    for (int i = 0; i < afc_input->nb_streams; ++i) {
        AVStream *stream = afc_input->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndexInput = i;
            videoinputStream = stream;
            inWidth = stream->codecpar->width;
            inHeight = stream->codecpar->height;
            LOGE(" inwidth %d , inheight %d ", inWidth, inHeight);
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndexInput = i;
            audioinputStream = stream;
        }
    }

//  videoCodecD
    if (videoCodecD == NULL) {
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    LOGE(" videoCodecD %s ", videoCodecD->name);
    vCtxD = avcodec_alloc_context3(videoCodecD);
    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }
    result = avcodec_parameters_to_context(vCtxD, afc_input->streams[videoIndexInput]->codecpar);
    if (result < 0) {
        LOGE(" avcodec_parameters_to_context Faild !");
        return -1;
    }
    result = avcodec_open2(vCtxD, videoCodecD, NULL);
    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }
    int64_t duration = (int64_t) (afc_input->duration * av_q2d(ffmpegTimeBase));
    if (duration > 60) {
        LOGE(" duration > 60 !");
        return -1;
    }

    LOGE(" inputduration %lld ", afc_input->duration);
    return 1;
}

int VideoRunBack::initOutput() {
    int result;
    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, outPath);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    result = addVideoOutputStream(inWidth, inHeight);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    result = addAudioOutputStream();
    if (result < 0) {
        LOGE(" addAudioOutputStream ");
        return -1;
    }

    return 1;
}


int VideoRunBack::addVideoOutputStream(int width, int height) {
    int result = 0;
    videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    videoIndexOutput = videoOutStream->index;
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    videoCodecE = avcodec_find_encoder(afot->video_codec);
    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return -1;
    }
    LOGE("video ENCODE NAME %s ", videoCodecE->name);
    vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }

    vCtxE->bit_rate = videoinputStream->codecpar->bit_rate;
    vCtxE->time_base = (AVRational) {1, outFrameRate};
    vCtxE->framerate = (AVRational) {outFrameRate, 1};
    vCtxE->gop_size = gopSize;
    vCtxE->max_b_frames = 1;
    vCtxE->pix_fmt = AV_PIX_FMT_YUV420P;
    vCtxE->codec_type = AVMEDIA_TYPE_VIDEO;
    vCtxE->width = width;
    vCtxE->height = height;

    result = avcodec_parameters_from_context(videoOutStream->codecpar, vCtxE);

    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }

    result = avcodec_open2(vCtxE, videoCodecE, NULL);

    if (result < 0) {
        LOGE("video Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO !");

    return 1;
}

void VideoRunBack::run() {


}

int VideoRunBack::addAudioOutputStream() {

    return 1;
}

AVFrame *VideoRunBack::deocdePacket(AVPacket *packet, AVCodecContext *decode) {

    int result = avcodec_send_packet(decode, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(decode, frame);
        if (result < 0) {
            LOGE(" avcodec_receive_frame  faild %s ", av_err2str(result));
            av_frame_free(&frame);
            return NULL;
        }
        return frame;
    }
    av_frame_free(&frame);
    return NULL;
}


AVPacket *VideoRunBack::encodeFrame(AVFrame *frame, AVCodecContext *encode) {
    if (frame == NULL || encode == NULL) {
        return NULL;
    }
    int result = 0;
    result = avcodec_send_frame(encode, frame);
    if (result < 0) {
        LOGE(" avcodec_send_frame faild ! %s ", av_err2str(result));
        return NULL;
    }
    AVPacket *packet = av_packet_alloc();
    while (result >= 0) {
        result = avcodec_receive_packet(encode, packet);
        if (result < 0) {
            LOGE(" avcodec_receive_packet faild ! %s ", av_err2str(result));
            av_packet_free(&packet);
            return NULL;
        }
        return packet;
    }
    av_packet_free(&packet);
    return NULL;
}


int VideoRunBack::startBackParse() {
    LOGE(" -------------------start------------------------ ");
    av_register_all();
#ifdef DEBUG
    av_log_set_callback(custom_log);
#endif
    int result = 0;
    char *tempYuv = "sdcard/FFmpeg/temp.yuv";
    FILE *fCache = fopen(tempYuv, "wb+");
    FILE *testFile = fopen("sdcard/FFmpeg/test.yuv", "wb");
    result = initInput();
    if (result < 0) {
        LOGE(" initInput faild ! ");
        return -1;
    }
    result = initOutput();
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return -1;
    }
    AVPacket *pkt = av_packet_alloc();
    int frameCount = 0;
    int64_t videoStreamDuration = 0;
    int64_t videoStartTime = 0;
    while (true) {
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            break;
        }

        if (pkt->stream_index == videoIndexInput) {
            if (videoStartTime == 0) {
                videoStartTime = pkt->pts;
            }
            frameCount++;
            videoStreamDuration = pkt->pts;
        }
    }
    frameDuration = videoStreamDuration / frameCount;
    LOGE(" frameCount %d , videoStreamDuration %lld  , frameDuration %d  , startTime %lld ",
         frameCount, videoStreamDuration, frameDuration, videoStartTime);

    int yuvSize = inWidth * inHeight * 3 / 2;
    char *readBuffer = (char *) malloc(yuvSize);

    int64_t firstGopPts = -1;
    int64_t nextKeyFramePts = videoStreamDuration;
    int backFrame = 3;
    result = av_seek_frame(afc_input, videoIndexInput,
                           (videoStreamDuration - backFrame * frameDuration), AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE(" av_seek_frame %s ", av_err2str(result));
        return -1;
    }
    int backFrameCount = 0;
    int writeFrame = 0;
    while (true) {
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            LOGE(" SEEK TO %lld \n\n\n", (firstGopPts - backFrame * frameDuration));
            result = av_seek_frame(afc_input, videoIndexInput,
                                   (firstGopPts - backFrame * frameDuration), AVSEEK_FLAG_BACKWARD);
            if (result < 0) {
                LOGE(" SEEK FAILD MAYBE FINISH ");
                return -1;
            }
            nextKeyFramePts = firstGopPts;
            firstGopPts = -1;
            continue;
        }
        if (pkt->stream_index == audioIndexInput) {
            continue;
        }
        if (pkt->stream_index == videoIndexInput) {
            gopFrameCount++;
            if (firstGopPts == -1) {
                firstGopPts = pkt->pts;
                LOGE(" FIRST PTS %lld ", firstGopPts);
            }
//            AVFrame *vFrame = deocdePacket(pkt, vCtxD);
            result = avcodec_send_packet(vCtxD, pkt);
            if (result < 0) {
                LOGE("  avcodec_send_packet %s ", av_err2str(result));
//                return NULL;
//                continue;
            }

            while (result >= 0) {
                AVFrame *vFrame = av_frame_alloc();
                result = avcodec_receive_frame(vCtxD, vFrame);
                if (result < 0) {
//                    LOGE(" avcodec_receive_frame  faild %s ", av_err2str(result));
                    av_frame_free(&vFrame);
                    vFrame = NULL;
//                    return NULL;
                }
//                return vFrame;
                if (vFrame != NULL) {
                    backFrameCount++;
                    writeFrame2File(vFrame, fCache);
                    av_frame_free(&vFrame);
                    vFrame = NULL;
                }
            }

//            if (vFrame != NULL) {
//                backFrameCount++;
//                writeFrame2File(vFrame, fCache);
//                av_frame_free(&vFrame);
//            }
        }
        if (pkt->pts >= nextKeyFramePts) {
            //完成了一个区间
            LOGE(" SEEK TO %lld \n\n\n", (firstGopPts - backFrame * frameDuration));
            result = av_seek_frame(afc_input, videoIndexInput,
                                   (firstGopPts - backFrame * frameDuration), AVSEEK_FLAG_BACKWARD);
            if (result < 0) {
                LOGE(" SEEK FAILD MAYBE FINISH ");
                return -1;
            }
            nextKeyFramePts = firstGopPts;
            firstGopPts = -1;

//            while (true) {
//                //清空其中的数据
//                result = avcodec_send_packet(vCtxD, NULL);
//                if(result < 0){
//                    LOGE(" send error %s " , av_err2str(result));
//                }
//                AVFrame *vFrame = av_frame_alloc();
//                result = avcodec_receive_frame(vCtxD, vFrame);
//                if(result < 0){
//                    av_frame_free(&vFrame);
//                    break;
//                }
//                else if(vFrame != NULL){
//                    backFrameCount++;
//                    LOGE(" FLUSH backFrameCount %d " , backFrameCount);
//                    writeFrame2File(vFrame, fCache);
//                    av_frame_free(&vFrame);
//                }
//            }7
//            LOGE(" backFrameCount %d " , backFrameCount);
            fflush(fCache);
//            ftell()
            //开始倒序读取
            while (true) {
                backFrameCount--;
                fseek(fCache, yuvSize * backFrameCount, SEEK_SET);
                fread(readBuffer, 1, yuvSize, fCache);
                fwrite(readBuffer, 1, yuvSize, testFile);
//                LOGE(" backFrameCount %d ", backFrameCount);
                writeFrame ++;
                if (backFrameCount <= 0) {
                    LOGE(" backFrameCount break %d " , backFrameCount);
                    break;
                }
            }
            backFrameCount = 0;
            fclose(fCache);
            fCache = fopen(tempYuv, "wb+");

            if (gopFrameCount >= frameCount) {
                break;
            }
        }
    }
    LOGE(" END write frame %d " ,writeFrame);
    return 1;
}

void VideoRunBack::writeFrame2File(AVFrame *vFrame, FILE *file) {
    gopCount ++;
    LOGE(" gopCount %d , pts %lld " , gopCount ,vFrame->pts );
    fwrite(vFrame->data[0], 1, vFrame->linesize[0] * inHeight, file);
    fwrite(vFrame->data[1], 1, vFrame->linesize[1] * inHeight / 2, file);
    fwrite(vFrame->data[2], 1, vFrame->linesize[2] * inHeight / 2, file);
}


VideoRunBack::~VideoRunBack() {

}