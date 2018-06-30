//
// Created by dugang on 2018/6/29.
//


#include "FFmpegReadFrame.h"
#include "my_log.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


void ReadFrame::run(){
    int result = 0;
    while (!isExit) {

        if (pause) {
            threadSleep(500);
            continue;
        }

        if (audioPktQue->size() >= maxAudioPacket  ) {
            //控制缓冲大小
            threadSleep(2);
            continue;
        }

        AVPacket *pkt_ = av_packet_alloc();
        result = av_read_frame(afc, pkt_);
        if (result < 0) {
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }
        if (pkt_->stream_index == audioIndex) {
            audioPktQue->push(pkt_);
        }   else {
            av_packet_free(&pkt_);
        }
    }
}
ReadFrame::ReadFrame(queue<AVPacket *> *queue  ,  AVFormatContext *s , int audioIndex){
    audioPktQue = queue;
    afc = s ;
    this->audioIndex = audioIndex;
}
ReadFrame::~ReadFrame(){

}