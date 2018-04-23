//
// Created by Administrator on 2018/4/21/021.
//
#include<iostream>
#include <my_log.h>
#include "decode_encode_test.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
}
using namespace std;

//extern "C"
//JNIEXPORT
//jint JNI_OnLoad(JavaVM *vm,void *res)
//{
//    av_jni_set_java_vm(vm,0);
//    return JNI_VERSION_1_4;
//}

int decode(const char *input_path, JNIEnv *env, jobject surface) {
    int result;
    av_register_all();
    avcodec_register_all();
    AVFormatContext *afc = NULL;
    result = avformat_open_input(&afc, input_path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return result;
    }
    result = avformat_find_stream_info(afc, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return result;
    }

    int video_index = 0;
    int audio_index = 0;
    AVCodec *audioCode = NULL;
    AVCodec *videoCode = NULL;
    AVCodecContext *ac = NULL;
    AVCodecContext *vc = NULL;

    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;
            LOGE("VIDEO WIDTH %d , HEIGHT %d , format %d ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format);
            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            if (!videoCode) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return -1;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audio_index = i;
            LOGE("VIDEO samplerate %d ", avStream->codecpar->sample_rate);
            audioCode = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!audioCode) {
                LOGE("audio avcodec_find_decoder FAILD!");
                return -1;
            }
        }
    }
    ac = avcodec_alloc_context3(audioCode);
    if (!ac) {
        LOGE("AC AVCodecContext FAILD ! ");
    }
    vc = avcodec_alloc_context3(videoCode);
    if (!vc) {
        LOGE("VC AVCodecContext FAILD ! ");
    }
    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc, afc->streams[video_index]->codecpar);
    avcodec_parameters_to_context(ac, afc->streams[audio_index]->codecpar);
    LOGE("AC sample_rate %d chnnel %d , pix format %d ",
         afc->streams[audio_index]->codecpar->sample_rate,
         afc->streams[audio_index]->codecpar->channels,
         afc->streams[audio_index]->codecpar->format);

    vc->thread_count = 4;
    ac->thread_count = 4;
    LOGE(" SUCCESS ");

    result = avcodec_open2(vc, 0, 0);
    if (result != 0) {
        LOGE("vc avcodec_open2 Faild !");
        return -1;
    }

    result = avcodec_open2(ac, 0, 0);
    if (result != 0) {
        LOGE("ac avcodec_open2 Faild !");
        return -1;
    }
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    SwsContext *sws = NULL;
    int outWidth = vc->width;
    int outHeight = vc->height;
    int frameCount = 0;

    char *rgb = new char[outWidth * outHeight * 4];
    char *pcm = new char[48000 * 4 * 2];

    FILE *fvout = fopen("sdcard/FFmpeg/tempout.rgba", "wb+");
    FILE *faout_l = fopen("sdcard/FFmpeg/temppcm_left.pcm", "wb+");
    FILE *faout_r = fopen("sdcard/FFmpeg/temppcm_right.pcm", "wb+");


    //音频重采样上下文初始化
    SwrContext *actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(1),
                              AV_SAMPLE_FMT_S16, ac->sample_rate,
                              av_get_default_channel_layout(ac->channels),
                              ac->sample_fmt, ac->sample_rate,
                              0, 0);
    result = swr_init(actx);
    if (result < 0) {
        LOGE(" swr_init FAILD !");
        return -1;
    }


    ANativeWindow *aWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(aWindow, outWidth, outHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer wbuf;

    while (true) {
        result = av_read_frame(afc, pkt);
        if (result < 0) {
            LOGE(" READ FRAME FAILD !");
            break;
        }

        frameCount++;
//        LOGE(" FRAME COUNT %d" , frameCount);
        AVCodecContext *tempCC = vc;
        if (pkt->stream_index == audio_index) {
            tempCC = ac;
        } else if (pkt->stream_index == video_index) {
            tempCC = vc;
        }
        result = avcodec_send_packet(tempCC, pkt);
        int p = pkt->pts;
        av_packet_unref(pkt);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            continue;
        }
        while (true) {
            result = avcodec_receive_frame(tempCC, frame);
            if (result < 0) {
                break;
            }
            if (tempCC == vc) {
                sws = sws_getCachedContext(sws,
                                           frame->width, frame->height,
                                           (AVPixelFormat) frame->format,
                                           outWidth, outHeight, AV_PIX_FMT_RGBA, SWS_FAST_BILINEAR,
                                           0, 0, 0);

                if (!sws) {
                    LOGE("sws_getCachedContext FAILD !");
                } else {
                    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                    data[0] = (uint8_t *) rgb;
                    int lines[AV_NUM_DATA_POINTERS] = {0};
                    lines[0] = outWidth * 4;
                    int h = sws_scale(sws, (const uint8_t **) frame->data, frame->linesize, 0,
                                      frame->height, data, lines);
                    LOGE("SLICE HEIGHT %d ", h);
                    ANativeWindow_lock(aWindow, &wbuf, 0);
                    uint8_t *dst = (uint8_t *) wbuf.bits;
                    memcpy(dst, rgb, outWidth * outHeight * 4);
                    ANativeWindow_unlockAndPost(aWindow);

                }
            } else if (tempCC == ac) {
                if (ac->channels == 2) {
//                    LOGE("LINESIZE[0] %d , linesize[1] %d " ,frame->linesize[0] , frame->linesize[1] );
                    uint8_t *out[1] = {0};
                    out[0] = (uint8_t *) pcm;
                    //音频重采样
                    int len = swr_convert(actx, out,
                                          frame->nb_samples,
                                          (const uint8_t **) frame->data,
                                          frame->nb_samples);
                    LOGE("frame->pkt_size %d frame->nb_samples %d ", frame->linesize[0],
                         frame->nb_samples);
                    //单声道
                    fwrite(out[0], 1, 2048, faout_l);
//                    fwrite(frame->data[1] , 1 ,frame->linesize[1] , faout_r );
                }
            }
        }
    }

    //清楚数组，如果没有括号将会造成内存泄露
    delete[] rgb;
    fclose(fvout);
    fclose(faout_l);
    fclose(faout_r);
    avformat_close_input(&afc);
    avcodec_free_context(&ac);
    avcodec_free_context(&vc);
    LOGE(" DECODE SUCCESS ");
    return result;
}


int encode(const char *input_yuv, const char *input_pcm) {


}









