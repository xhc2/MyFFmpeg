//
// Created by Administrator on 2018/7/3/003.
//

#include <my_log.h>
#include "DecodeVideoThread.h"

void DecodeVideoThread::run(){

}

void DecodeVideoThread::update(MyData mydata){
    if(!mydata.isAudio){
        pthread_mutex_lock(&mutex_pthread);
        LOGE(" videoPktQue.size() %d " , videoPktQue.size());
        if(videoPktQue.size() < maxPackage){
            videoPktQue.push(mydata.pkt);
            pthread_mutex_unlock(&mutex_pthread);
        }
        pthread_mutex_unlock(&mutex_pthread);
    }
}