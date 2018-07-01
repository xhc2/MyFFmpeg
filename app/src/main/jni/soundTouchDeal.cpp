//
// Created by Administrator on 2018/6/30/030.
//

#include <my_log.h>
#include "soundTouchDeal.h"


SoundTouchDeal::SoundTouchDeal(int sampleRate , queue<MyData> *audioFrameQue){
    soundTouch = new SoundTouch();
    //采样率
    soundTouch->setSampleRate(sampleRate);
    //声道数
    soundTouch->setChannels(1);
    //速度
    soundTouch->setTempo(1.0);
    //声调
    soundTouch->setPitch(1);
    this->audioFrameQue = audioFrameQue;
    fFile = fopen("sdcard/FFmpeg/fbfore.pcm" ,"wb+");
}



int SoundTouchDeal::dealPcm(SAMPLETYPE **getBuf ){
    int size = 0;
    int returnSize = 0;
    while (true){
        putBuf = NULL;
        if(!audioFrameQue->empty()){
            MyData myData = audioFrameQue->front();
            audioFrameQue->pop();
            size = myData.size;
            putBuf = myData.data;
            fwrite(putBuf , 1 , size , fFile);
            free(myData.data);
        }


        if(finish){
            finish = false;
            if(size > 0){
                soundTouch->putSamples((SAMPLETYPE *)putBuf , size / 2);
                returnSize = soundTouch->receiveSamples(*getBuf, size / 2);
            }else{
                soundTouch->flush();
            }
        }
        if(returnSize == 0){
            finish = true;
            continue;
        }
        else {
            if(putBuf == NULL){
                returnSize = soundTouch->receiveSamples(*getBuf, size / 2);
                if(returnSize == 0){
                    finish = true;
                    continue;
                }
            }
            LOGE(" returnSize %d " ,returnSize );
            return returnSize * 2;
        }
    }

    return 1;
}

SoundTouchDeal::~SoundTouchDeal(){

}