//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_MYSOUNDTOUCH_H
#define MYFFMPEG_MYSOUNDTOUCH_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <my_data.h>
#include "MyThread.h"
#include "FFmpegReadFrame.h"
#include "DecodeAudioThread.h"
#include <queue>
#include <SoundTouch.h>


extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

using namespace std;
using namespace soundtouch;

class mySoundTouch : public MyThread{

public :
    void run();
    SoundTouch *soundTouch;
    //ffmepg
    AVFrame *aframe;
    AVFormatContext *afc;
    int audioindex = -1;
    AVCodec *audioCode;
    AVCodecContext *ac;
    SwrContext *swc;
    int64_t duration;
    //audio_sl
    SLObjectItf engineOpenSL = NULL;
    SLPlayItf iplayer = NULL;
    SLEngineItf eng = NULL;
    SLObjectItf mix = NULL;
    SLObjectItf player = NULL;
    SLAndroidSimpleBufferQueueItf pcmQue = NULL;


    queue<AVPacket *> audioPktQue;
    queue<MyData> audioFrameQue;
    SAMPLETYPE *buf_play_gpu = NULL;
    void init(const char *soucetouch);
    char *playAudioBuffer;
    SoundTouchDeal *soundTouchDeal;

private :
    int initFFmpeg(const char *inputPath);
    int initOpenSl();
    SLEngineItf createOpenSL();
    void audioPlayDelay();
    ReadFrame *readFrameThread;
    DecodeAudioThread *decodeAudioThread;
    pthread_mutex_t mutex_pthread = PTHREAD_MUTEX_INITIALIZER;
    int sampleRate ;


};

#endif //MYFFMPEG_MYSOUNDTOUCH_H
