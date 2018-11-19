//
// Created by dugang on 2018/7/5.
//

#include <my_log.h>
#include <unistd.h>
#include "AudioPlayer.h"


AudioPlayer::AudioPlayer(int simpleRate , int channel){
    this->simpleRate = simpleRate ;
    this->channel = channel;
    playAudioTemp = (char *)malloc(1024 * 2 * channel);
    maxFrame = 140;
    pts = 0;
    sonicRead = new SonicRead(simpleRate , channel , 1.0f, &audioFrameQue , &mutex_pthread);
    initAudio();
}

void AudioPlayer::run(){
    audioPlayDelay();
}
//暂停或者播放
int AudioPlayer::pause_audio(bool myPauseFlag) {
    if (iplayer != NULL) {
        SLresult re = (*iplayer)->SetPlayState(iplayer, myPauseFlag ? SL_PLAYSTATE_PAUSED
                                                                            : SL_PLAYSTATE_PLAYING);
        if (re != SL_RESULT_SUCCESS) {
            LOGE("SetPlayState pause FAILD ");
            return -1;
        }
        LOGE("SetPlayState pause success ");
    }
    return 0;
}

void AudioPlayer::pauseAudio(){
    (*pcmQue)->Clear(pcmQue);
    pause_audio(true);
}

void AudioPlayer::playAudio(){
    pause_audio(false);
}

void AudioPlayer::audioPlayDelay() {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    pthread_mutex_lock(&mutex_pthread);
    int a = 1;
    while(!isExit && !pause){
        if(!audioFrameQue.empty()){
            (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
            (*pcmQue)->Enqueue(pcmQue, &a, 1);
            break;
        }
        threadSleep(2);
    }
    pthread_mutex_unlock(&mutex_pthread);
}

void AudioPlayer::update(MyData *mydata){
    if(mydata->data == NULL || mydata->size <= 0 || !mydata->isAudio){
        return ;
    }
    while(!isExit){
        if(pause){
            break;
        }

        if(audioFrameQue.size() < maxFrame){
            audioFrameQue.push(mydata);
            break;
        }
        else{
            threadSleep(1);
            continue;
        }
    }
}

AudioPlayer::~AudioPlayer(){
    LOGE(" AudioPlayer destory ! start ");
    if(playAudioTemp != NULL){
        free(playAudioTemp);
    }
    pts = 0;
    if (iplayer && (*iplayer)) {
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    if (pcmQue != NULL) {
        (*pcmQue)->Clear(pcmQue);
    }
    if (player != NULL) {
        (*player)->Destroy(player);
        player = NULL;
        iplayer = NULL;
        pcmQue = NULL;
    }

    if (mix != NULL) {
        (*mix)->Destroy(mix);
        mix = NULL;
    }
    if (engineOpenSL != NULL) {
        (*engineOpenSL)->Destroy(engineOpenSL);
        engineOpenSL = NULL;
        eng = NULL;
    }
    if(sonicRead != NULL){
        delete sonicRead;
    }
    LOGE("AudioPlayer destory ! ");
}


void AudioPlayer::clearQue(){
    LOGE(" AudioPlayer::clearQue ");
    while(!isExit){

        if(!audioFrameQue.empty()){
            MyData *myData = audioFrameQue.front();
            if(myData !=NULL){
                myData->drop();
            }
            audioFrameQue.pop();
            continue;
        }
        break;
    }
}

void AudioPlayer::seekStart() {

}
void AudioPlayer::seekFinish() {

}
int AudioPlayer::sonicFlush(){
    return sonicRead->sonicFlush();
}

void AudioPlayer::changeSpeed(float speed){
    sonicRead->changeSpeed(speed);
}

void audioPlayerCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    AudioPlayer *ap = (AudioPlayer *)context;
//    if(!ap->audioFrameQue.empty()){
//        MyData *myData = ap->audioFrameQue.front();
//        ap->audioFrameQue.pop();
//        ap->pts = myData->pts;
//        memcpy(ap->playAudioTemp , myData->data , myData->size);
//        if(myData->size > 0){
//            (*bf)->Enqueue(bf,ap->playAudioTemp , myData->size );
//        }
//        delete  myData;
//    }

    int size = ap->sonicRead->dealAudio( &ap->getBuf ,  ap->pts);


    if(size > 0 &&  ap->getBuf != NULL){
        (*bf)->Enqueue(bf, ap->getBuf  , size );
    }
}

int AudioPlayer::initAudio() {
    //创建引擎
    eng = createOpenSL();
    if (!eng) {
        LOGE("createSL FAILD ");
    }
    //2.创建混音器
    mix = NULL;
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};

    //音频格式
    SLDataFormat_PCM pcm_ = {
            SL_DATAFORMAT_PCM,
            1,
            getSimpleRate(simpleRate) ,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm_};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                       sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return RESULT_FAILD;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue)->RegisterCallback(pcmQue, audioPlayerCallBack, this);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;
}

SLuint32 AudioPlayer::getSimpleRate(int sampleRate){

    switch (sampleRate){
        case 44100:
            return SL_SAMPLINGRATE_44_1;
        case 48000:
            return SL_SAMPLINGRATE_48;

    }

    return SL_SAMPLINGRATE_48;
}

SLEngineItf AudioPlayer::createOpenSL() {
    SLresult re = 0;
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

