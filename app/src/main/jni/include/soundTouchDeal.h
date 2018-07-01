//
// Created by Administrator on 2018/6/30/030.
//

#ifndef MYFFMPEG_SOUNDTOUCHDEAL_H
#define MYFFMPEG_SOUNDTOUCHDEAL_H

#include "SoundTouch.h"
#include "my_data.h"
#include <queue>

using namespace std;
using namespace soundtouch;

class SoundTouchDeal{

private :
    SoundTouch *soundTouch;
    bool finish = true;
    queue<MyData> *audioFrameQue;
    char *putBuf;
    FILE *fFile;

public :
    SoundTouchDeal(int sampleRate , queue<MyData> *audioFrameQue);
    ~SoundTouchDeal();
    int dealPcm( SAMPLETYPE **getBuf);

};

#endif //MYFFMPEG_SOUNDTOUCHDEAL_H
