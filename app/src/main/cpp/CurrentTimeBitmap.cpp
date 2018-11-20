//
// Created by Administrator on 2018/11/20/020.
//

#include "CurrentTimeBitmap.h"


CurrentTimeBitmap::CurrentTimeBitmap(const char *path, int outWidth, int outHeight) {
    this->outWidth = outWidth;
    this->outHeight = outHeight;
    int len = strlen(path);
    len++;
    this->path = (char *) malloc(len);
    strcpy(this->path, path);
    int result = initFFmpeg(path);
    if (result < 0) {
        LOGE(" init ffmepg faild !");
        return;
    }
}


AVFrame *CurrentTimeBitmap::deocdePacket(AVPacket *packet) {

    int result = avcodec_send_packet(vc, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(vc, frame);
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

int CurrentTimeBitmap::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outWidth, outHeight,
                         AV_PIX_FMT_RGB565BE, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {
        return -1;
    }
    return 1;
}

char* CurrentTimeBitmap::getCurrentBitmap(float time) {
    int result = av_seek_frame(afc, -1,
                               (time / 1000) * AV_TIME_BASE * afc->start_time,
                               AVSEEK_FLAG_BACKWARD);
    int64_t pts = 0;
//    FILE *out = fopen("sdcard/FFmpeg/test.bmp" , "wb+");
    while (true) {
        AVPacket *packet = av_packet_alloc();
        result = av_read_frame(afc, packet);
        if (result < 0) {
            LOGE(" ********************** READ FRAME FAILD *********************");
            av_packet_free(&packet);
            break;
        }
        if (packet->stream_index == video_index) {
            AVFrame *frame = deocdePacket(packet);
            if (frame != NULL) {
                pts = (int64_t) (frame->pts * av_q2d(afc->streams[video_index]->time_base) *
                                 1000);


                if ((pts >= time * 1000)) {
                    sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                              0, frame->height, outVFrame->data, outVFrame->linesize);
                    LOGE(" FIND FRAME  linesize %d , height %d " , outVFrame->linesize[0] , outVFrame->height);
//                    fwrite(outVFrame->data[0] , 1 , outVFrame->linesize[0] * outVFrame->height ,out );
//                    fclose(out);
                    char bmp[outVFrame->linesize[0] * outVFrame->height] ;

                    av_frame_free(&frame);
                    break;
                }
            }

            av_packet_free(&packet);
        }
    }


}


int CurrentTimeBitmap::initFFmpeg(const char *path) {

    int result = 0;
    av_register_all();
    LOGE(" play path %s ", path);
    afc = NULL;
    result = avformat_open_input(&afc, path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }
    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d ,pix format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            initSwsContext(avStream->codecpar->width, avStream->codecpar->height,
                           avStream->codecpar->format);
            if (!videoCode) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }

    if (videoCode == NULL) {
        return RESULT_FAILD;
    }

    LOGE(" video code name %s  ", videoCode->name);


    vc = avcodec_alloc_context3(videoCode);
    if (!vc) {
        LOGE("vc AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc, afc->streams[video_index]->codecpar);

    vc->thread_count = 4;

    result = avcodec_open2(vc, NULL, NULL);
    if (result != 0) {
        LOGE("vc avcodec_open2 Faild !");
        return RESULT_FAILD;
    }
    outVFrame = av_frame_alloc();
    outVFrame->width = outWidth;
    outVFrame->height = outHeight;
    outVFrame->format = AV_PIX_FMT_YUV420P;
    result = av_frame_get_buffer(outVFrame, 0);
    if (result < 0) {
        LOGE(" av_frame_get_buffer FAILD ! ");
        return -1;
    }
    result = av_frame_make_writable(outVFrame);
    if (result < 0) {
        LOGE(" av_frame_make_writable FAILD ! ");
        return -1;
    }
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}


CurrentTimeBitmap::~CurrentTimeBitmap() {

}