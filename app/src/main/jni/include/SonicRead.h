//
// Created by dugang on 2018/7/2.
//

#ifndef MYFFMPEG_SONICREAD_H
#define MYFFMPEG_SONICREAD_H

#include "SonicRead.h"
#include "sonic.h"
#include "my_data.h"
#include <queue>
using namespace std;
class SonicRead{



private :
    sonicStream tempoStream;
    queue<MyData> *audioFrameQue;
    short *playAudioBuffer;
    short *getAudioBuffer ;
    bool isExit = false;
    int putBufferSize;
    int getBufferSize;

public :
    SonicRead(int samplerate , int channel , float speed , queue<MyData> *audioFrameQue);
    ~SonicRead();
    void putSample(short *buf , int size);
    void sonicFlush();
    int availableBytes();
    int reciveSample(short *getBuf , int lenByte);
    int dealAudio(short **getBuf);
};

#endif //MYFFMPEG_SONICREAD_H
