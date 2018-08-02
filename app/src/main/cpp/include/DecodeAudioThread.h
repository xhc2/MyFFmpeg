//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_DECODEAUDIOTHREAD_H
#define MYFFMPEG_DECODEAUDIOTHREAD_H

#include <queue>
#include "MyThread.h"
#include "my_data.h"
#include "Utils.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
}

using namespace std;
class DecodeAudioThread : public MyThread{

public :
    void run();
    DecodeAudioThread(queue<MyData> *audioFrameQue , queue<AVPacket *> *audioPktQue ,
                      AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex  , SwrContext *swc, AVFrame *aframe);
    DecodeAudioThread(queue<MyData> *audioFrameQue , queue<AVPacket *> *audioPktQue ,
                      AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex  );
    ~DecodeAudioThread();
private :
    queue<MyData> *audioFrameQue;
    queue<AVPacket *> *audioPktQue;

    int maxFrame;
    AVCodecContext *ac;
    AVFormatContext *afc;
    int audioIndex;
    char *play_audio_temp;
    AVFrame *aframe;
    SwrContext *swc;
    Utils utils;
};

#endif //MYFFMPEG_DECODEAUDIOTHREAD_H
