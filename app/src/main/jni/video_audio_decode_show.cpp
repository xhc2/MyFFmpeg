//
// Created by dugang on 2018/4/25.
//
#include <jni.h>
#include <my_log.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include "video_audio_decode_show.h"
SLObjectItf engineOpenSL = NULL;
SLPlayItf iplayer_ = NULL;
SLEngineItf eng_ =  NULL;
SLObjectItf mix_ =  NULL;
SLObjectItf  player_ = NULL;
FILE *filePcm ;
char *buf_ = NULL;
SLAndroidSimpleBufferQueueItf  pcmQue_ = NULL;

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

void pcmCallBack(SLAndroidSimpleBufferQueueItf bf , void *context){


    if(!buf_)
    {
        buf_ = new char[1024*1024];
    }
    if(!filePcm)
    {
        filePcm = fopen("sdcard/FFmpeg/test_sl.pcm","rb");
    }
    if(!filePcm){
        LOGE("file faild !");
        return;
    }
    if(feof(filePcm) == 0)
    {
        int len = fread(buf_,1,1024,filePcm);
        if(len > 0)
            //往缓冲区中丢数据，有数据他就播放。没有数据就进入回调函数
            (*bf)->Enqueue(bf,buf_,len);
    }
}


int play_audio_stream() {
//    pcm_path = path;
    //创建引擎
    eng_ = createOpenSL();
    if(!eng_){
        LOGE("createSL FAILD ");
    }

    //2.创建混音器
    mix_ = NULL;
    SLresult re = 0;
    re = (*eng_)->CreateOutputMix(eng_ , &mix_ , 0 ,0 , 0);
    if(re != SL_RESULT_SUCCESS){
        LOGE("CreateOutputMix FAILD ");
        return -1;
    }
    re = (*mix_)->Realize(mix_, SL_BOOLEAN_FALSE);
    if(re != SL_RESULT_SUCCESS){
        LOGE("Realize FAILD ");
        return -1;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX , mix_};
    SLDataSink audioSink = {&outmix , 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE , 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,//    声道数
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que,&pcm};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng_)->CreateAudioPlayer(eng_ , &player_ , &ds , &audioSink , sizeof(ids) / sizeof(SLInterfaceID) , ids , req);
    if(re != SL_RESULT_SUCCESS){
        LOGE("CreateAudioPlayer FAILD ");
        return -1;
    }
    (*player_)->Realize(player_,SL_BOOLEAN_FALSE);
    re = (*player_)->GetInterface(player_ , SL_IID_PLAY , &iplayer_);
    if(re != SL_RESULT_SUCCESS){
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return -1;
    }
    re = (*player_)->GetInterface(player_ , SL_IID_BUFFERQUEUE , &pcmQue_);
    if(re != SL_RESULT_SUCCESS){
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue_)->RegisterCallback(pcmQue_ , pcmCallBack , 0);

    (*iplayer_)->SetPlayState(iplayer_ , SL_PLAYSTATE_PLAYING);

    (*pcmQue_)->Enqueue(pcmQue_ , "" , 1);
    LOGE(" play_audio SUCCESS ");
    return 1;
}
int audioDestroy(){
    if(player_ != NULL){
        (*player_)->Destroy(player_);
        player_ = NULL;
        iplayer_ = NULL;
        pcmQue_ = NULL;
    }
    if(mix_ != NULL){
        (*mix_)->Destroy(mix_);
        mix_ = NULL;
    }
    if(engineOpenSL != NULL){
        (*engineOpenSL)->Destroy(engineOpenSL);
        engineOpenSL = NULL;
        eng_ = NULL;
    }
    if(filePcm != NULL){
        fclose(filePcm);
        filePcm = NULL;
    }
    return 1;
}

int initOpenSlEs() {
    return play_audio_stream();
}

int videoAudioOpen(JNIEnv *env, jobject surface, const char *path) {
    LOGE("videoAudioOpen %s  ", path);
    int result = -1;
    result = initOpenSlEs();
    return result;
}
