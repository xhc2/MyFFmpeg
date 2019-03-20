//
// Created by Administrator on 2018/11/28/028.
//

#include "NewAudioPlayer.h"


NewAudioPlayer::NewAudioPlayer(int sampleRate, int channelCount) {
    finishFlag = false;
    bqPlayerObject = NULL;
    engineObject = NULL;
    outputMixObject = NULL;
    getBuf = NULL;
    outputMixEnvironmentalReverb = NULL;
    this->sampleRate = sampleRate;
    this->channelCount = channelCount;
    maxFrame = 140;
    pts = 0;
    createEngine();
    createBufferQueueAudioPlayer();
    sonicRead = new SonicRead(sampleRate, channelCount, 1.0f, &audioFrameQue );
}

void NewAudioPlayer::createEngine() {

    SLresult result;
    // create engine 创建接口对象
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(" slCreateEngine faild ! ");
        return;
    }

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(" (*engineObject)->Realize faild ! ");
        return;
    }

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("  (*engineObject)->GetInterface faild ! ");
        return;
    }

    // create output mix, with environmental reverb specified as a non-required interface 设置混音器
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};

    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("  (*engineEngine)->CreateOutputMix faild ! ");
        return;
    }

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("  (*outputMixObject)->Realize faild ! ");
        return;
    }

    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,  &outputMixEnvironmentalReverb);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("  (*outputMixObject)->GetInterface faild ! ");
        return;
    }

    if (SL_RESULT_SUCCESS == result) {
        const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }
    LOGE(" create engine success !");
}

void NewAudioPlayer::run() {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    audioPlayDelay();
}


void NewAudioPlayer::audioPlayDelay() {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    pthread_mutex_lock(&mutex_pthread);
    int a = 1;
    while (!isExit && !pause) {
        if (!audioFrameQue.empty()) {
            (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
            (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, &a, 1);
            break;
        }
        threadSleep(2);
    }
    pthread_mutex_unlock(&mutex_pthread);
}


void audioPlayCallBack2(SLAndroidSimpleBufferQueueItf bf, void *context) {
    NewAudioPlayer *ap = (NewAudioPlayer *) context;
    int size = ap->sonicRead->dealAudio(&ap->getBuf, ap->pts);
    if(ap->getBuf == NULL){
        return ;
    }
    if (size > 0 &&  ap->getBuf != NULL) {
        (*bf)->Enqueue(bf, ap->getBuf, size);
    }
}


SLuint32 NewAudioPlayer::getSimpleRate(int sampleRate) {

    switch (sampleRate) {
        case 8000:
            return SL_SAMPLINGRATE_8;
        case 11025:
            return SL_SAMPLINGRATE_11_025;
        case 12000:
            return SL_SAMPLINGRATE_12;
        case 16000:
            return SL_SAMPLINGRATE_16;
        case 44100:
            return SL_SAMPLINGRATE_44_1;
        case 48000:
            return SL_SAMPLINGRATE_48;
        default:
            LOGE("xxxxxxxxxxxxxxxxxxxxxxxxxxx audio player getSimpleRate faild ! xxxxxxxxxxxxxxxxxxxxxxxxxxx");
            break;
    }

    return SL_SAMPLINGRATE_48;
}


void NewAudioPlayer::createBufferQueueAudioPlayer() {
    SLresult result;

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //单声道，44100
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                   1,
                                   getSimpleRate(sampleRate),
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT  ,
                                   SL_BYTEORDER_LITTLEENDIAN};

//    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
//                                   2,
//                                   getSimpleRate(sampleRate),
//                                   SL_PCMSAMPLEFORMAT_FIXED_16,
//                                   SL_PCMSAMPLEFORMAT_FIXED_16,
//                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
//                                   SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource audioSrc = {&loc_bufq, &format_pcm};


    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(" CreateAudioPlayer FAILD !");
        return ;
    }

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("  (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE); FAILD !");
        return ;
    }

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("  result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay); FAILD !");
        return ;
    }

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("  (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, FAILD !");
        return ;
    }
    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, audioPlayCallBack2,
                                                      this);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("  (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,  FAILD !");
        return ;
    }

    LOGE(" create player success !");
}


void NewAudioPlayer::update(MyData *mydata) {
    if (mydata == NULL) {
        finishFlag = true;
//        LOGE(" audio player que.size %d ", audioFrameQue.size());
        if (audioFrameQue.size() <= 0) {
            pts = -100;
        }
        return;
    }
    if (mydata->data == NULL || mydata->size <= 0 || !mydata->isAudio) {
        return;
    }
    while (!isExit) {
        if (pause) {
            mydata->drop();
            break;
        }
        if (audioFrameQue.size() < maxFrame) {
            audioFrameQue.push(mydata);
            break;
        } else {
            threadSleep(1);
            continue;
        }
    }
}

void NewAudioPlayer::changeSpeed(float speed) {
    sonicRead->changeSpeed(speed);
}

void NewAudioPlayer::pauseAudio() {
    (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
    pausePlay(true);
}

void NewAudioPlayer::clearQue() {
    while (!isExit) {

        if (!audioFrameQue.empty()) {
            MyData *myData = audioFrameQue.front();
            if (myData != NULL) {
                myData->drop();
            }
            audioFrameQue.pop();
            continue;
        }
        break;
    }
    LOGE(" AudioPlayer::clearQue ");
}


//暂停或者播放
int NewAudioPlayer::pausePlay(bool myPauseFlag) {
    if (bqPlayerPlay != NULL) {
        SLresult re = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, myPauseFlag ? SL_PLAYSTATE_PAUSED
                                                                              : SL_PLAYSTATE_PLAYING);
        if (re != SL_RESULT_SUCCESS) {
            LOGE("SetPlayState pause FAILD ");
            return -1;
        }
        LOGE("SetPlayState pause success ");
    }
    return 0;
}

NewAudioPlayer::~NewAudioPlayer() {
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    if (bqPlayerBufferQueue != NULL) {
        (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
    }
    if (sonicRead != NULL) {
//        dealAudio();
        delete sonicRead;
        sonicRead = NULL;
    }
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
    }
    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    clearQue();


    LOGE("  release audioPlayer success ! ");
}