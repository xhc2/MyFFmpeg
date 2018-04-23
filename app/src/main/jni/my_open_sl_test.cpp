//
// Created by Administrator on 2018/4/23/023.
//
#include "my_log.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include "my_open_sl_test.h"
/**
 * 利用opensles 播放pcm原始数据文件
 */
static SLObjectItf engineSL = NULL;
const char * pcm_path;
SLEngineItf createSL() {
    SLresult re = NULL;
    SLEngineItf en = NULL;
    re = slCreateEngine(&engineSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return NULL;
    }
    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return NULL;
    }
    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return NULL;
    }

    return en;
}

void pcmCall(SLAndroidSimpleBufferQueueItf bf , void *context){
    LOGD("PcmCall");
    static FILE *fp = NULL;
    static char *buf = NULL;
    if(!buf)
    {
        buf = new char[1024*1024];
    }
    if(!fp)
    {
        fp = fopen(pcm_path,"rb");
    }
    if(!fp)return;
    if(feof(fp) == 0)
    {
        int len = fread(buf,1,1024,fp);
        if(len > 0)
            (*bf)->Enqueue(bf,buf,len);
    }
}

int play_audio(const char *path) {
    pcm_path = path;
    //创建引擎
    SLEngineItf eng = createSL();
    if(!eng){
        LOGE("createSL FAILD ");
    }

    //2.创建混音器
    SLObjectItf mix = NULL;
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng , &mix , 0 ,0 , 0);
    if(re != SL_RESULT_SUCCESS){
        LOGE("CreateOutputMix FAILD ");
        return -1;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if(re != SL_RESULT_SUCCESS){
        LOGE("Realize FAILD ");
        return -1;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX , mix};
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
    SLObjectItf  player = NULL;
    SLPlayItf iplayer = NULL;
    SLAndroidSimpleBufferQueueItf  pcmQue = NULL;
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng , &player , &ds , &audioSink , sizeof(ids)/ sizeof(SLInterfaceID) , ids , req);
    if(re != SL_RESULT_SUCCESS){
        LOGE("CreateAudioPlayer FAILD ");
        return -1;
    }
    (*player)->Realize(player,SL_BOOLEAN_FALSE);
    re = (*player)->GetInterface(player , SL_IID_PLAY , &iplayer);
    if(re != SL_RESULT_SUCCESS){
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return -1;
    }
    re = (*player)->GetInterface(player , SL_IID_BUFFERQUEUE , &pcmQue);
    if(re != SL_RESULT_SUCCESS){
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue)->RegisterCallback(pcmQue , pcmCall , 0);

    (*iplayer)->SetPlayState(iplayer , SL_PLAYSTATE_PLAYING);

    (*pcmQue)->Enqueue(pcmQue , "" , 1);
    LOGE(" play_audio SUCCESS ");
    return 1;
}


