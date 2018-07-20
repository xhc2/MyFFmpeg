//
// Created by dugang on 2018/7/2.
//

#ifndef MYFFMPEG_SONICREAD_H
#define MYFFMPEG_SONICREAD_H

#include "SonicRead.h"
#include "sonic.h"
#include "my_data.h"
#include <queue>
#include <pthread.h>
using namespace std;
class SonicRead{



private :
    sonicStream tempoStream;
    queue<MyData *> *audioFrameQue;
    short *playAudioBuffer;
    short *getAudioBuffer ;
    bool isExit;
    int putBufferSize;
    int getBufferSize;
    int sampleRate ;
    int channel;
    pthread_mutex_t *mutex_pthread;
public :
    SonicRead(int samplerate , int channel , float speed , queue<MyData *> *audioFrameQue ,pthread_mutex_t *mutex_pthread);
    ~SonicRead();
    void putSample(short *buf , int size);
    int sonicFlush();
    int availableBytes();
    void changeSpeed(float speed);
    int reciveSample(short *getBuf , int lenByte);
    int dealAudio(short **getBuf , int64_t  &pts);
    void destroySonicRead();
    void createSonicRead();
};

#endif //MYFFMPEG_SONICREAD_H
