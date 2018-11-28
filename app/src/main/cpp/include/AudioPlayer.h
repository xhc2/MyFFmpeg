//
// Created by dugang on 2018/7/5.
//

#ifndef MYFFMPEG_AUDIOPLAYER_H
#define MYFFMPEG_AUDIOPLAYER_H
#include <GLES2/gl2.h>
#include <SLES/OpenSLES_Android.h>
#include "my_data.h"
#include "Notify.h"
#include "MyThread.h"
#include "SonicRead.h"
#include <queue>

class AudioPlayer : public Notify , public MyThread{
private :

    SLObjectItf engineOpenSL ;
    SLPlayItf iplayer ;
    SLEngineItf eng ;
    SLObjectItf mix ;
    SLObjectItf player ;
    SLAndroidSimpleBufferQueueItf pcmQue;


    int simpleRate ;
    int channel;
    int maxFrame;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
//
    void audioPlayDelay();
    int pause_audio(bool myPauseFlag);
    SLuint32 getSimpleRate(int sampleRate);
    int initAudio();
    int createOpenSL();
    int createAudioPlayer();
    int createMix();

    int sonicFlush();
    void seekStart();
    void seekFinish();
public :
    std::queue<MyData *> audioFrameQue;
    int64_t pts;
    short *getBuf;
    SonicRead *sonicRead;
    bool finishFlag ;
    char *playAudioTemp;


    void pauseAudio();
    void playAudio();
    AudioPlayer(int simpleRate , int channel);
    ~AudioPlayer();
    void changeSpeed(float speed);
    void update(MyData *mydata);
    void run();
    void clearQue();


};

#endif //MYFFMPEG_AUDIOPLAYER_H
