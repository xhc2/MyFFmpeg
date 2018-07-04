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
        LOGE("XHC READFREAME %d " , result);
        if (result < 0) {
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }
        MyData myData;
        myData.pkt = pkt_ ;
        if (pkt_->stream_index == audioIndex) {
            myData.isAudio = true;
        } else if (pkt_->stream_index == videoIndex) {
            myData.isAudio = false;
        } else {
            av_packet_free(&pkt_);
            pkt_ =  NULL;
            myData.drop();
        }
        if(pkt_ != NULL){
            notify(myData);
        }
    }
}
void ReadAVPackage::update(MyData mydata){

}

ReadAVPackage::~ReadAVPackage(){

}