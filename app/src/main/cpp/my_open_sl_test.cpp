//
// Created by Administrator on 2018/4/23/023.
//
/**
 *      1、创建接口对象
        2、设置混音器
        3、创建播放器（录音器）
        4、设置缓冲队列和回调函数
        5、设置播放状态
        6、启动回调函数
 */

#include "my_log.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <assert.h>
#include "my_open_sl_test.h"

using namespace std;
queue<char *> pcmTempQue;
// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;
// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;

//static short *nextBuffer;
//static unsigned nextSize;

void createEngine() {
    SLresult result;
    // create engine 创建接口对象
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // create output mix, with environmental reverb specified as a non-required interface 设置混音器
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;


    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);

    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    LOGE(" create engine success !");
}


//创建播放器
void createBufferQueueAudioPlayer() {

    SLresult result;
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //单声道，44100
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,
                                   2,
                                   SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    if(SL_RESULT_SUCCESS != result){
        LOGE(" CreateAudioPlayer FAILD !");
    }

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        LOGE("  (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE); FAILD !");
    }

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if(SL_RESULT_SUCCESS != result){
        LOGE("  result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay); FAILD !");
    }

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    if(SL_RESULT_SUCCESS != result){
        LOGE("  (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, FAILD !");
    }
    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    if(SL_RESULT_SUCCESS != result){
        LOGE("  (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,  FAILD !");
    }

    // set the player's state to playing
//    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
//    assert(SL_RESULT_SUCCESS == result);
//    (void)result;

    LOGE(" create player success !");
}


// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {

    if(pcmTempQue.size() > 0){
        char *buffer = pcmTempQue.front();
        pcmTempQue.pop();
        SLresult result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, 2048);
    }


    // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
    // which for this code example would indicate a programming error
//    if (SL_RESULT_SUCCESS != result) {
//        pthread_mutex_unlock(&audioEngineLock);
//    }

    // for streaming playback, replace this test by logic to find and fill the next buffer
//    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
//        SLresult result;
//        // enqueue another buffer
//        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
//        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
//        // which for this code example would indicate a programming error
//        if (SL_RESULT_SUCCESS != result) {
//            pthread_mutex_unlock(&audioEngineLock);
//        }
//        (void)result;
//    } else {
//        releaseResampleBuf();
//        pthread_mutex_unlock(&audioEngineLock);
//    }
}

void startPlayTest() {
    FILE *Fpcm = fopen("sdcard/FFmpeg/test_2c_441_16.pcm", "r");
    int count = 0;
    while (true) {
        count++;
        char *buffer = (char *) malloc(2048);
        int len = fread(buffer, 1, 2048, Fpcm);
        LOGE(" len %d " , len );
        if (len != 2048) {

            break;
        }
        pcmTempQue.push(buffer);
        if(count = 2){
            SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
            if(result != SL_RESULT_SUCCESS){
                LOGE("XXXXXXXXXXXXXXXXXXXXX PLAY FAILD !XXXXXXXXXXXXXXXXXXXXXXX");
            }
        }
    }
    LOGE(" --------------END-------------- ");
}