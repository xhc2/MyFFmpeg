//
// Created by Administrator on 2018/11/28/028.
//

#ifndef MYFFMPEG_NEWAUDIOPLAYER_H
#define MYFFMPEG_NEWAUDIOPLAYER_H
#include "my_log.h"
#include "my_data.h"
#include "MyThread.h"
#include "SonicRead.h"
#include "Notify.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>




class NewAudioPlayer : public MyThread  , public Notify {
private :

    SLObjectItf engineObject;
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject ;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb ;
    SLObjectItf bqPlayerObject ;
    SLPlayItf bqPlayerPlay;

    int sampleRate ;
    int channelCount;
    int maxFrame;
    bool finishFlag ;

    void createEngine();
    void createBufferQueueAudioPlayer();
    void audioPlayDelay();
    SLuint32 getSimpleRate(int sampleRate);
    int pausePlay(bool myPauseFlag);

public :
    short *getBuf;
    SonicRead *sonicRead;
    int64_t pts;
    std::queue<MyData *> audioFrameQue;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    void  pauseAudio();
    virtual void run();
    void update(MyData *mydata);
    void changeSpeed(float speed);
    void clearQue();
    NewAudioPlayer(int sampleRate , int channelCount);
    ~NewAudioPlayer();
};

#endif //MYFFMPEG_NEWAUDIOPLAYER_H
