//
// Created by Administrator on 2018/7/3/003.
//

#include <my_data.h>
#include <my_log.h>
#include "ReadAvPacket.h"


ReadAVPackage::ReadAVPackage(AVFormatContext *afc  , int audioIndex , int videoIndex){
    this->afc = afc;
    this->audioIndex = audioIndex;
    this->videoIndex = videoIndex;
}

void ReadAVPackage::run(){
    int result = 0;
    while (!isExit) {

        if (pause) {
            threadSleep (500);
            continue;
        }
        AVPacket *pkt_ = av_packet_alloc();
        result = av_read_frame(afc, pkt_);

        if (result < 0) {
            LOGE(" READ PACKAGE FAILD ");
            stop();//停止线程
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }
        MyData *myData = new MyData();
        myData->pkt = pkt_ ;
        myData->size = pkt_->size;
        if (pkt_->stream_index == audioIndex) {
            myData->isAudio = true;
        } else if (pkt_->stream_index == videoIndex) {
            myData->isAudio = false;
        } else {
            av_packet_free(&pkt_);
            pkt_ =  NULL;
            delete myData;
        }
        if(pkt_ != NULL){
            notify(myData);
        }
    }
}
//不需要被通知。只是实现一个抽象方法
void ReadAVPackage::update(MyData *mydata){

}

ReadAVPackage::~ReadAVPackage(){

}