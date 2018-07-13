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
    vframe = av_frame_alloc();
    this->videoIndex = videoIndex;
//    fileYuv = fopen("sdcard/FFmpeg/fileyuv" , "wb+");
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
        if (!pck) {
            LOGE(" video packet null !");
            videoPktQue.pop();
            continue;
        }
        //音视频同步处理
        int64_t pts = util.getConvertPts(pck->pts, afc->streams[videoIndex]->time_base);
//        LOGE("tong bu apts %lld , vpts %lld ", apts, pts);
        if (pts >= apts) {
            threadSleep(1);
            continue;
        }

        videoPktQue.pop();
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
            MyData *myData = new MyData();
            myData->pts = pts;
            myData->vWidth = vc->width ;
            myData->vHeight = vc->height;
            int size = vc->width *  vc->height;
            //y
            myData->datas[0] = (uint8_t *)malloc(size);
            //u
            myData->datas[1] = (uint8_t *)malloc(size / 4);
            //v
            myData->datas[2] = (uint8_t *)malloc(size / 4);

            memcpy(myData->datas[0] ,vframe->data[0] , size );
            memcpy(myData->datas[1] ,vframe->data[1] , size / 4 );
            memcpy(myData->datas[2] ,vframe->data[2] , size / 4);

//            fwrite(myData->datas[0] ,1 ,size , fileYuv);
//            fwrite(myData->datas[1] ,1 ,size / 4  , fileYuv);
//            fwrite(myData->datas[2] ,1 ,size / 4  , fileYuv);

            this->notify(myData);
        }
    }
}

void DecodeVideoThread::update(MyData *mydata) {
    if (mydata->isAudio) return ;
    while (true) {
            pthread_mutex_lock(&mutex_pthread);
            if (videoPktQue.size() < maxPackage) {
                videoPktQue.push(mydata->pkt);
                pthread_mutex_unlock(&mutex_pthread);
                break;
            }
            else{
                threadSleep(2);
                pthread_mutex_unlock(&mutex_pthread);
            }
    }
}

DecodeVideoThread::~DecodeVideoThread(){

}