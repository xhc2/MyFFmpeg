//
// Created by Administrator on 2018/7/3/003.
//

#include <my_log.h>
#include "DecodeVideoThread.h"

DecodeVideoThread::DecodeVideoThread(AVFormatContext *afc , AVCodecContext  *vc  ,int videoIndex){
    pthread_mutex_init(&mutex_pthread , NULL);
    maxPackage = 100;
    this->afc = afc;
    this->vc = vc;
    this->videoIndex = videoIndex;
}

void DecodeVideoThread::run() {
    int result;

    while (!isExit) {
        //测试代码
        if (pause) {
            threadSleep(50);
            continue;
        }

        if (videoPktQue.empty()) {
            threadSleep(2);
            continue;
        }
        AVPacket *pck = videoPktQue.front();
//        LOGE(" pop pck %lld " , pck->pts);
        //音视频同步处理
        int64_t pts = util.getConvertPts(pck->pts, afc->streams[pck->stream_index]->time_base);
//        LOGE("tong bu apts %lld , vpts %lld ", apts, pts);
        if (pts > apts) {
            threadSleep(1);
            continue;
        }

        videoPktQue.pop();
        if (!pck) {
            LOGE(" video packet null !");
            continue;
        }

        result = avcodec_send_packet(vc, pck);
        av_packet_free(&pck);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            continue;
        }


        while (true) {
            result = avcodec_receive_frame(vc, vframe);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
                break;
            } else if (result < 0) {
                break;
            }


            vframe->pts = util.getConvertPts(vframe->pts,
                                            afc->streams[videoIndex]->time_base);
            pts = vframe->pts;
            MyData myData ;
            myData.pts = pts;
            myData.vWidth = vc->width ;
            myData.vHeight = vc->height;
            memcpy(myData.datas ,vframe->data , sizeof(myData.datas) );
            this->notify(myData);
        }
    }

}

void DecodeVideoThread::update(MyData mydata) {
    if (mydata.isAudio) return ;
    while (true) {
            pthread_mutex_lock(&mutex_pthread);
            if (videoPktQue.size() < maxPackage) {
                videoPktQue.push(mydata.pkt);
                pthread_mutex_unlock(&mutex_pthread);
                break;
            }
            else{
                pthread_mutex_unlock(&mutex_pthread);
                threadSleep(2);
            }
    }
}

DecodeVideoThread::~DecodeVideoThread(){

}