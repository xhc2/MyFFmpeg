//
// Created by Administrator on 2018/7/3/003.
//

#ifndef MYFFMPEG_MYAUDIO_H
#define MYFFMPEG_MYAUDIO_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class MyAudio{
private:
    //audio_sl
    SLObjectItf engineOpenSL ;
    SLPlayItf iplayer ;
    SLEngineItf eng ;
    SLObjectItf mix ;
    SLObjectItf player ;
    SLAndroidSimpleBufferQueueItf pcmQue ;
    SLEngineItf createOpenSL();

    ~MyAudio();
    int initAudio();
public :
    MyAudio();
};

#endif //MYFFMPEG_MYAUDIO_H
