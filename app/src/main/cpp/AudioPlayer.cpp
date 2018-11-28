//
// Created by dugang on 2018/7/5.
//

#include <my_log.h>
#include <unistd.h>
#include "AudioPlayer.h"


AudioPlayer::AudioPlayer(int simpleRate, int channel) {
    finishFlag = false;
//    reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    this->simpleRate = simpleRate;
    this->channel = channel;
    playAudioTemp = (char *) malloc(1024 * 2 * channel);
    maxFrame = 140;
    pts = 0;
    sonicRead = new SonicRead(simpleRate, channel, 1.0f, &audioFrameQue );
    initAudio();
}

void AudioPlayer::run() {
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

void AudioPlayer::pauseAudio() {
    (*pcmQue)->Clear(pcmQue);
    pause_audio(true);
}

void AudioPlayer::playAudio() {
    pause_audio(false);
}

void AudioPlayer::audioPlayDelay() {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    pthread_mutex_lock(&mutex_pthread);
    int a = 1;
    while (!isExit && !pause) {
        if (!audioFrameQue.empty()) {
            (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
            (*pcmQue)->Enqueue(pcmQue, &a, 1);
            break;
        }
        threadSleep(2);
    }
    pthread_mutex_unlock(&mutex_pthread);
}

void AudioPlayer::update(MyData *mydata) {
    if (mydata == NULL) {
        finishFlag = true;
        LOGE(" audio player que.size %d ", audioFrameQue.size());
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


void AudioPlayer::clearQue() {
    LOGE(" AudioPlayer::clearQue ");
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
}

void AudioPlayer::seekStart() {

}

void AudioPlayer::seekFinish() {

}

int AudioPlayer::sonicFlush() {
    return sonicRead->sonicFlush();
}

void AudioPlayer::changeSpeed(float speed) {
    sonicRead->changeSpeed(speed);
}

void audioPlayerCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    AudioPlayer *ap = (AudioPlayer *) context;
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

    int size = ap->sonicRead->dealAudio(&ap->getBuf, ap->pts);
//    LOGE(" AUDIO PLAY SIZE %d , finishflag %d" , size , ap->finishFlag);
//    if (size == -100 && ap->finishFlag) {
//        ap->pts = -100;
//        return;
//    }

    if (size > 0 && ap->getBuf != NULL) {
        (*bf)->Enqueue(bf, ap->getBuf, size);
    }
}


int AudioPlayer::createAudioPlayer() {
    SLresult re = 0;
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};

    //音频格式
    SLDataFormat_PCM pcm_ = {
            SL_DATAFORMAT_PCM,
            2,
            getSimpleRate(simpleRate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
//    SLDataFormat_PCM pcm_ = {
//            SL_DATAFORMAT_PCM,
//            1,
//            getSimpleRate(simpleRate),
//            SL_PCMSAMPLEFORMAT_FIXED_16,
//            SL_PCMSAMPLEFORMAT_FIXED_16,
//            SL_SPEAKER_FRONT_LEFT ,
//            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
//    };

    SLDataSource ds = {&que, &pcm_};

    //创建播放器
//    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
//    const SLboolean req[] = {SL_BOOLEAN_TRUE};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   3, ids, req);
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
    return RESULT_SUCCESS;
}

int AudioPlayer::createMix() {
    //2.创建混音器
    mix = NULL;
    SLresult re = 0;

    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    re = (*eng)->CreateOutputMix(eng, &mix, 1, ids, req);
//    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix)->GetInterface(mix, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    LOGE(" RESULT %d " , re);
    if (re != SL_RESULT_SUCCESS) {
        LOGE(" (*mix)->GetInterface(mix) FAILD ");
        return RESULT_FAILD;
    }
    if (SL_RESULT_SUCCESS == re) {
        SLEnvironmentalReverbSettings reverbSettings  = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
        re = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }
    return RESULT_SUCCESS;
}

int AudioPlayer::initAudio() {
    //创建引擎
    int result = createOpenSL();
    if (!eng || result < 0) {
        LOGE("createSL FAILD ");
        return RESULT_FAILD;
    }
    result = createMix();
    if (result < 0) {
        LOGE("createMix FAILD ");
        return RESULT_FAILD;
    }
    result = createAudioPlayer();
    if (result < 0) {
        LOGE("createAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;
}

SLuint32 AudioPlayer::getSimpleRate(int sampleRate) {

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

int AudioPlayer::createOpenSL() {
    SLresult re = 0;
//    SLEngineItf en = NULL;

    re = slCreateEngine(&engineOpenSL, 0, 0, 0, 0, 0);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return -1;
    }

    re = (*engineOpenSL)->Realize(engineOpenSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return -1;
    }

    re = (*engineOpenSL)->GetInterface(engineOpenSL, SL_IID_ENGINE, &eng);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return -1;
    }
    return 0;
}


AudioPlayer::~AudioPlayer() {

    pts = 0;
    if (iplayer && (*iplayer)) {
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    if (pcmQue != NULL) {
        (*pcmQue)->Clear(pcmQue);
    }
    LOGE("      delete sonicRead; ");
    if (sonicRead != NULL) {
        delete sonicRead;
    }

    LOGE("       (*player)->Destroy(player); ");
    if (player != NULL) {
        //当视频播放完毕，这里有点问题,会被阻塞掉
        (*player)->Destroy(player);
        player = NULL;
        iplayer = NULL;
        pcmQue = NULL;
    }
    LOGE("      (*mix)->Destroy(mix); ");
    if (mix != NULL) {
        (*mix)->Destroy(mix);
        mix = NULL;
    }
    LOGE("    (*engineOpenSL)->Destroy(engineOpenSL); ");
    if (engineOpenSL != NULL) {
        (*engineOpenSL)->Destroy(engineOpenSL);
        engineOpenSL = NULL;
        eng = NULL;
    }

    if (playAudioTemp != NULL) {
        free(playAudioTemp);
    }
    LOGE(" AudioPlayer destory success ! ");
}
