//
// Created by xhc on 2018/5/10.
//
#include <my_log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "gpu_video_sl_audio.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

//ffmepg
AVFrame *aframe_gpu;
AVFrame *vframe_gpu;
AVFormatContext *afc_gpu;
int video_index_gpu, audio_index_gpu;
AVCodec *videoCode_gpu, *audioCode_gpu;
AVCodecContext *ac_gpu, *vc_gpu;
int outWidth_gpu, outHeight_gpu;
//char *pcm_gpu;
SwrContext *swc_gpu;
int videoDuration_gpu;

//audio_sl
SLObjectItf engineOpenSL_gpu = NULL;
SLPlayItf iplayer_gpu = NULL;
SLEngineItf eng_gpu = NULL;
SLObjectItf mix_gpu = NULL;
SLObjectItf player_gpu = NULL;
SLAndroidSimpleBufferQueueItf pcmQue_gpu = NULL;


int initFFmpeg_gpu(const char *input_path) {

    int result = 0;
    av_register_all();
    avcodec_register_all();

    aframe_gpu = av_frame_alloc();
    vframe_gpu = av_frame_alloc();
    LOGE(" input path %s ", input_path);
    result = avformat_open_input(&afc_gpu, input_path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_open_input FAILD !");
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc_gpu, 0);


    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return RESULT_FAILD;
    }

    videoDuration_gpu = afc_gpu->duration / (AV_TIME_BASE / 1000);

    LOGE(" video duration %d ", videoDuration_gpu);

    for (int i = 0; i < afc_gpu->nb_streams; ++i) {
        AVStream *avStream = afc_gpu->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index_gpu = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d , format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode_gpu = avcodec_find_decoder(avStream->codecpar->codec_id);
            if (!videoCode_gpu) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audio_index_gpu = i;
            LOGE("audio samplerate %d ", avStream->codecpar->sample_rate);
            audioCode_gpu = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!audioCode_gpu) {
                LOGE("audio avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }

    ac_gpu = avcodec_alloc_context3(audioCode_gpu);
    if (!ac_gpu) {
        LOGE("ac_gpu AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    vc_gpu = avcodec_alloc_context3(videoCode_gpu);
    if (!vc_gpu) {
        LOGE("vc_gpu AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc_gpu, afc_gpu->streams[video_index_gpu]->codecpar);
    avcodec_parameters_to_context(ac_gpu, afc_gpu->streams[audio_index_gpu]->codecpar);

    vc_gpu->thread_count = 4;
    ac_gpu->thread_count = 4;

    result = avcodec_open2(vc_gpu, 0, 0);
    if (result != 0) {
        LOGE("vc_gpu avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    result = avcodec_open2(ac_gpu, 0, 0);
    if (result != 0) {
        LOGE("ac_gpu avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    outWidth_gpu = vc_gpu->width;
    outHeight_gpu = vc_gpu->height;

    LOGE("outwidth %d , outheight %d ", outWidth_gpu, outHeight_gpu);

    //音频重采样上下文初始化 , AV_SAMPLE_FMT_S16 格式的单声道
    swc_gpu = swr_alloc();
    swc_gpu = swr_alloc_set_opts(swc_gpu,
                                 av_get_default_channel_layout(1),
                                 AV_SAMPLE_FMT_S16, ac_gpu->sample_rate,
                                 av_get_default_channel_layout(ac_gpu->channels),
                                 ac_gpu->sample_fmt, ac_gpu->sample_rate,
                                 0, 0);
    result = swr_init(swc_gpu);
    if (result < 0) {
        LOGE(" swr_init FAILD !");
        return RESULT_FAILD;
    }
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}


SLEngineItf createOpenSL_gpu() {
    SLresult re = 0;
    SLEngineItf en = NULL;

    re = slCreateEngine(&engineOpenSL_gpu, 0, 0, 0, 0, 0);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return NULL;
    }

    re = (*engineOpenSL_gpu)->Realize(engineOpenSL_gpu, SL_BOOLEAN_FALSE);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return NULL;
    }

    re = (*engineOpenSL_gpu)->GetInterface(engineOpenSL_gpu, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return NULL;
    }
    return en;
}

void pcmCallBack_gpu(SLAndroidSimpleBufferQueueItf bf, void *context) {
//    if (!audioFrameQue.empty()) {
//        MyData myData ;
//        myData = audioFrameQue.front();
//        audioFrameQue.pop();
//        memcpy(audio_buf_ , myData.data , myData.size);
//        (*bf)->Enqueue(bf, audio_buf_ , myData.size);
//        apts = myData.pts;
//        free(myData.data);
//    }
}

int initAudio_gpu() {
    //创建引擎
    eng_gpu = createOpenSL_gpu();
    if (!eng_gpu) {
        LOGE("createSL FAILD ");
    }

    //2.创建混音器
    mix_gpu = NULL;
    SLresult re = 0;
    re = (*eng_gpu)->CreateOutputMix(eng_gpu, &mix_gpu, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix_gpu)->Realize(mix_gpu, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix_gpu};
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
    re = (*eng_gpu)->CreateAudioPlayer(eng_gpu, &player_gpu, &ds, &audioSink,
                                       sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    (*player_gpu)->Realize(player_gpu, SL_BOOLEAN_FALSE);
    re = (*player_gpu)->GetInterface(player_gpu, SL_IID_PLAY, &iplayer_gpu);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return RESULT_FAILD;
    }
    re = (*player_gpu)->GetInterface(player_gpu, SL_IID_BUFFERQUEUE, &pcmQue_gpu);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }
//    audio_buf_ = new unsigned char[1024 * 1024];
    (*pcmQue_gpu)->RegisterCallback(pcmQue_gpu, pcmCallBack_gpu, 0);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;
}


int open_gpu(const char *path, jobject win) {
    int result = initFFmpeg_gpu(path);
    if (RESULT_FAILD == result) {
        LOGE(" initFFmpeg_gpu ");
        return RESULT_FAILD;
    }
    result = initAudio_gpu();
    if (RESULT_FAILD == result) {
        LOGE(" initAudio_gpu ");
        return RESULT_FAILD;
    }
    return RESULT_SUCCESS;
}

int playOrPause_gpu() {

    return 1;
}

//就是暂停
int justPause_gpu() {

    return 1;
}

int seek_gpu(double radio) {

    return 1;
}

int destroy_FFmpeg() {

    if (aframe_gpu != NULL) {
        LOGE(" av_frame_free aframe_gpu ");
        av_frame_free(&aframe_gpu);
    }
    if (vframe_gpu != NULL) {
        LOGE(" av_frame_free vframe_gpu ");
        av_frame_free(&vframe_gpu);
    }
    if (vc_gpu != NULL) {
        LOGE(" avcodec_close vc_gpu ");
        avcodec_close(vc_gpu);
    }
    if (ac_gpu != NULL) {
        LOGE(" avcodec_close ac_gpu ");
        avcodec_close(ac_gpu);
    }
    if (afc_gpu != NULL) {
        LOGE(" avformat_free_context afc_gpu ");
        avformat_free_context(afc_gpu);
        afc_gpu = NULL;
    }
    if (swc_gpu != NULL) {
        LOGE(" swr_close swc_gpu ");
        swr_close(swc_gpu);
    }

    return 1;
}

int destroy_Audio() {
    if (iplayer_gpu && (*iplayer_gpu)) {
        (*iplayer_gpu)->SetPlayState(iplayer_gpu, SL_PLAYSTATE_STOPPED);
    }
    if (pcmQue_gpu != NULL) {
        (*pcmQue_gpu)->Clear(pcmQue_gpu);
    }
    if (player_gpu != NULL) {
        (*player_gpu)->Destroy(player_gpu);
        player_gpu = NULL;
        iplayer_gpu = NULL;
        pcmQue_gpu = NULL;
        LOGE("audio player_gpu destory ! ");
    }


    if (mix_gpu != NULL) {
        (*mix_gpu)->Destroy(mix_gpu);
        mix_gpu = NULL;
        LOGE("audio mix_gpu destory ! ");
    }
    if (engineOpenSL_gpu != NULL) {
        (*engineOpenSL_gpu)->Destroy(engineOpenSL_gpu);
        engineOpenSL_gpu = NULL;
        eng_gpu = NULL;
        LOGE("audio engineOpenSL_gpu destory ! ");
    }
    return 1;
}

int destroy_gpu() {
    destroy_FFmpeg();
    destroy_Audio();
    return 1;
}

