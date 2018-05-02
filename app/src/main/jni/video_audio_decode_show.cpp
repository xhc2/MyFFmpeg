//
// Created by dugang on 2018/4/25.
//
#include <jni.h>
#include <my_log.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include "video_audio_decode_show.h"
#include <queue>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "my_data.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
using namespace std;


queue<MyData> audioQue;
queue<MyData> yuvQue;

SLObjectItf engineOpenSL = NULL;
SLPlayItf iplayer_ = NULL;
SLEngineItf eng_ = NULL;
SLObjectItf mix_ = NULL;
SLObjectItf player_ = NULL;

//FILE *filePcm ;
char *buf_ = NULL;
SLAndroidSimpleBufferQueueItf pcmQue_ = NULL;
//设置缓冲区大小
int maxSize = 100;
bool playFlag = false;


SLEngineItf createOpenSL() {
    SLresult re = NULL;
    SLEngineItf en = NULL;
    re = slCreateEngine(&engineOpenSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return NULL;
    }
    re = (*engineOpenSL)->Realize(engineOpenSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return NULL;
    }
    re = (*engineOpenSL)->GetInterface(engineOpenSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return NULL;
    }
    return en;
}

void pcmCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {


    if (!buf_) {
        buf_ = new char[1024 * 1024];
    }
    LOGE(" ADD BUFFER audioQue.size %d ", audioQue.size());
    if (!audioQue.empty()) {
        MyData myData ;

        myData = audioQue.front();

        audioQue.pop();
        (*bf)->Enqueue(bf, myData.data, 2048);
    }
}


int play_audio_stream() {
//    pcm_path = path;
    //创建引擎
    eng_ = createOpenSL();
    if (!eng_) {
        LOGE("createSL FAILD ");
    }

    //2.创建混音器
    mix_ = NULL;
    SLresult re = 0;
    re = (*eng_)->CreateOutputMix(eng_, &mix_, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix_)->Realize(mix_, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix_};
    SLDataSink audioSink = {&outmix, 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm_ = {
            SL_DATAFORMAT_PCM,
            1,//    声道数
            SL_SAMPLINGRATE_48,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm_};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng_)->CreateAudioPlayer(eng_, &player_, &ds, &audioSink,
                                    sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    (*player_)->Realize(player_, SL_BOOLEAN_FALSE);
    re = (*player_)->GetInterface(player_, SL_IID_PLAY, &iplayer_);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return RESULT_FAILD;
    }
    re = (*player_)->GetInterface(player_, SL_IID_BUFFERQUEUE, &pcmQue_);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue_)->RegisterCallback(pcmQue_, pcmCallBack, 0);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;
}

//播放或者暂停音频
int playOrPauseAudio() {
    if (iplayer_ != NULL) {

        SLresult re = (*iplayer_)->SetPlayState(iplayer_, playFlag == true ? SL_PLAYSTATE_PLAYING
                                                                           : SL_PLAYSTATE_PAUSED);
        if (playFlag) {
            (*pcmQue_)->Enqueue(pcmQue_, "", 1);
        }
        if (re != SL_RESULT_SUCCESS) {
            LOGE("SetPlayState pause FAILD ");
            return RESULT_FAILD;
        }
        LOGE("SetPlayState pause success ");
    }
    return RESULT_SUCCESS;
}

int audioDestroy() {
    if (player_ != NULL) {
        (*player_)->Destroy(player_);
        player_ = NULL;
        iplayer_ = NULL;
        pcmQue_ = NULL;
    }
    if (mix_ != NULL) {
        (*mix_)->Destroy(mix_);
        mix_ = NULL;
    }
    if (engineOpenSL != NULL) {
        (*engineOpenSL)->Destroy(engineOpenSL);
        engineOpenSL = NULL;
        eng_ = NULL;
    }
    return 1;
}

int initOpenSlEs() {
    return play_audio_stream();
}

AVCodec *audioCode_ = NULL;
AVCodec *videoCode_ = NULL;
AVCodecContext *ac_ = NULL;
AVCodecContext *vc_ = NULL;
int video_index_ = 0;
int audio_index_ = 0;
AVFormatContext *afc_ = NULL;
AVPacket *pkt_ = NULL;
AVFrame *frame_ = NULL;
SwsContext *sws_ = NULL;
char *pcm_ = NULL;
SwrContext *actx_ = NULL;
char *rgb_ = NULL;
int outWidth_ = 0;
int outHeight_ = 0;
ANativeWindow_Buffer wbuf_;
ANativeWindow *aWindow;

int initWindow(JNIEnv *env, jobject surface) {
    aWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(aWindow, outWidth_, outHeight_, WINDOW_FORMAT_RGBA_8888);
    return RESULT_SUCCESS;
}

int initFFmpeg(const char *input_path) {

    int result = 0;
    av_register_all();
    avcodec_register_all();

    pkt_ = av_packet_alloc();
    frame_ = av_frame_alloc();

    result = avformat_open_input(&afc_, input_path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc_, 0);

    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return RESULT_FAILD;
    }

    for (int i = 0; i < afc_->nb_streams; ++i) {
        AVStream *avStream = afc_->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index_ = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d , format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode_ = avcodec_find_decoder(avStream->codecpar->codec_id);
            if (!videoCode_) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audio_index_ = i;
            LOGE("audio samplerate %d ", avStream->codecpar->sample_rate);
            audioCode_ = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!audioCode_) {
                LOGE("audio avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }
    ac_ = avcodec_alloc_context3(audioCode_);
    if (!ac_) {
        LOGE("ac_ AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }
    vc_ = avcodec_alloc_context3(videoCode_);
    if (!vc_) {
        LOGE("vc_ AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }
    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc_, afc_->streams[video_index_]->codecpar);
    avcodec_parameters_to_context(ac_, afc_->streams[audio_index_]->codecpar);

    vc_->thread_count = 4;
    ac_->thread_count = 4;
    LOGE(" SUCCESS ");

    result = avcodec_open2(vc_, 0, 0);
    if (result != 0) {
        LOGE("vc_ avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    result = avcodec_open2(ac_, 0, 0);
    if (result != 0) {
        LOGE("ac_ avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    outWidth_ = vc_->width;
    outHeight_ = vc_->height;
    int frameCount = 0;

    rgb_ = new char[outWidth_ * outHeight_ * 4];
    pcm_ = new char[48000 * 4 * 2];

    //音频重采样上下文初始化
    actx_ = swr_alloc();
    actx_ = swr_alloc_set_opts(actx_,
                               av_get_default_channel_layout(1),
                               AV_SAMPLE_FMT_S16, ac_->sample_rate,
                               av_get_default_channel_layout(ac_->channels),
                               ac_->sample_fmt, ac_->sample_rate,
                               0, 0);
    result = swr_init(actx_);
    if (result < 0) {
        LOGE(" swr_init FAILD !");
        return RESULT_FAILD;
    }

    return RESULT_SUCCESS;
}

//测试函数
void testPlay() {
    playFlag = true;
    playOrPauseAudio();
}

int videoAudioOpen(JNIEnv *env, jobject surface, const char *path) {
    LOGE("videoAudioOpen %s  ", path);
    int result = -1;
    result = initOpenSlEs();
    if (result == RESULT_FAILD) {
        LOGE(" initOpenSlEs FAILD ! ");
        return RESULT_FAILD;
    }
    result = initFFmpeg(path);
    if (result == RESULT_FAILD) {
        LOGE(" initFFmpeg FAILD ! ");
        return RESULT_FAILD;
    }
    result = initWindow(env, surface);
    if (result == RESULT_FAILD) {
        LOGE(" initWindow FAILD ! ");
        return RESULT_FAILD;
    }

    //控制音频的播放，音频里面有缓冲数据的时候，并且播放状态是playing的时候才能开始播放，后面正式的时候用解码线程，和播放线程应该能解决这个问题
    int audioCount = 0;

    while (true) {
        result = av_read_frame(afc_, pkt_);
        if (result < 0) {
            LOGE(" READ frame_ FAILD !");
            break;
        }

        AVCodecContext *tempCC = vc_;
        if (pkt_->stream_index == audio_index_) {
            tempCC = ac_;
        } else if (pkt_->stream_index == video_index_) {
            tempCC = vc_;
        }
        result = avcodec_send_packet(tempCC, pkt_);
        int p = pkt_->pts;
        av_packet_unref(pkt_);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            continue;
        }
        while (true) {
            result = avcodec_receive_frame(tempCC, frame_);
            if (result < 0) {
                break;
            }
            if (tempCC == vc_) {
                sws_ = sws_getCachedContext(sws_,
                                            frame_->width, frame_->height,
                                            (AVPixelFormat) frame_->format,
                                            outWidth_, outHeight_, AV_PIX_FMT_RGBA,
                                            SWS_FAST_BILINEAR,
                                            0, 0, 0);

                if (!sws_) {
                    LOGE("sws_getCachedContext FAILD !");
                } else {
                    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                    data[0] = (uint8_t *) rgb_;
                    int lines[AV_NUM_DATA_POINTERS] = {0};
                    lines[0] = outWidth_ * 4;

                    int h = sws_scale(sws_, (const uint8_t **) frame_->data, frame_->linesize, 0,
                                      frame_->height, data, lines);

//                    ANativeWindow_lock(aWindow, &wbuf_, 0);
//                    uint8_t *dst = (uint8_t *) wbuf_.bits;
//                    memcpy(dst, rgb_, outWidth_ * outHeight_ * 4);
//                    ANativeWindow_unlockAndPost(aWindow);
                }
            } else if (tempCC == ac_) {
                uint8_t *out[1] = {0};
                out[0] = (uint8_t *) pcm_;
                //音频重采样
                int len = swr_convert(actx_, out,
                                      frame_->nb_samples,
                                      (const uint8_t **) frame_->data,
                                      frame_->nb_samples);
//                    LOGE("frame_->pkt_size %d frame_->nb_samples %d ", frame_->linesize[0] , frame_->nb_samples);
                //音频部分需要自己维护一个缓冲区，通过他自己回调的方式处理
                char *pcm_temp = new char[48000 * 4 * 2];
                memcpy(pcm_temp, pcm_, 2048);
                MyData myData ;
                myData.data = pcm_temp;
                myData.isAudio = true;
                audioQue.push(myData);
                audioCount++;
                //要先缓冲几帧才能播放音频数据
                if (audioCount == 5) {
                    playFlag = true;
                    playOrPauseAudio();
                }
            }
        }

    }
    return RESULT_SUCCESS;
}
