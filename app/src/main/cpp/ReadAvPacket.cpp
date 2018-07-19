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
            threadSleep (4);
            LOGE(" AV READ PAUSE "  );
            continue;
        }

        AVPacket *pkt_ = av_packet_alloc();
        if(pkt_ == NULL){
            LOGE("READ FRAME av_packet_alloc FAILD !");
            continue;
        }
        result = av_read_frame(afc, pkt_);
        if (result < 0) {
            LOGE(" READ FRAME faild %s ", av_err2str(result));
            if(strcmp("End of file"  , av_err2str(result)) == 0){
                //文件结尾
//                LOGE(" READ PACKAGE FAILD %s " , av_err2str(result));
            }
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
            delete myData;
            myData = NULL;
        }
        if(myData != NULL){
            notify(myData);
        }
    }
    LOGE(" READ FRAME END !!!!!!!!!!!!!!!");
}


//不需要被通知。只是实现一个抽象方法
void ReadAVPackage::update(MyData *mydata){

}

ReadAVPackage::~ReadAVPackage(){

}