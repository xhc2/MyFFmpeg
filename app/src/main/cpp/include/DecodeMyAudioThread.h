//
// Created by dugang on 2018/7/4.
//

#ifndef MYFFMPEG_DECODEMYAUDIOTHREAD_H
#define MYFFMPEG_DECODEMYAUDIOTHREAD_H
#include <queue>

#include "MyThread.h"
#include "my_data.h"
#include "Utils.h"
#include "Notify.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
}

class DeocdeMyAudioThread : public MyThread ,public Notify {

private:
    void run();
    queue<AVPacket *> audioPktQue;
    int maxPackage ;
    AVFormatContext *afc;
    int audioIndex;
    SwrContext *swc;
    AVCodecContext *ac ;
    AVFrame *aframe;
    uint8_t *play_audio_temp;
    Utils utils;
    bool finishFlag ;

public :
    DeocdeMyAudioThread( AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex );
    ~DeocdeMyAudioThread();
    void setQueue(queue<AVPacket *> aq);
    void update(MyData *myData);
    void clearQue();
};

#endif //MYFFMPEG_DECODEMYAUDIOTHREAD_H
