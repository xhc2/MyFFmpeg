//
// Created by Administrator on 2018/6/30/030.
//

#ifndef MYFFMPEG_SOUNDTOUCHDEAL_H
#define MYFFMPEG_SOUNDTOUCHDEAL_H

#include "SoundTouch.h"

using namespace soundtouch;

class SoundTouchDeal{

private :
    SoundTouch *soundTouch;
    bool finish = true;
public :
    SoundTouchDeal(int sampleRate);
    ~SoundTouchDeal();
    int dealPcm(SAMPLETYPE *putBuf ,int bufSize, SAMPLETYPE **getBuf);

};

#endif //MYFFMPEG_SOUNDTOUCHDEAL_H
