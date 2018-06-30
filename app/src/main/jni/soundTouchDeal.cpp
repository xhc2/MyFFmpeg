//
// Created by Administrator on 2018/6/30/030.
//

#include "soundTouchDeal.h"


SoundTouchDeal::SoundTouchDeal(int sampleRate){
    soundTouch = new SoundTouch();
    //采样率
    soundTouch->setSampleRate(sampleRate);
    //声道数
    soundTouch->setChannels(1);
    //速度
    soundTouch->setTempo(1.0);
    //声调
    soundTouch->setPitch(1);
}

int SoundTouchDeal::dealPcm(SAMPLETYPE *buf,int bufSize ,  SAMPLETYPE **getBuf ){
    int size;

    while (true){
        if(buf == NULL){
            return 0;
        }
        if(finish){
            finish = false;
            if(bufSize > 0){
                soundTouch->putSamples(buf , bufSize / 2);
                size = soundTouch->receiveSamples(*getBuf, bufSize / 2);
            }else{
                soundTouch->flush();
            }
        }
        if(size == 0){
            finish = true;
            continue;
        }

    }

    return 1;
}

SoundTouchDeal::~SoundTouchDeal(){

}