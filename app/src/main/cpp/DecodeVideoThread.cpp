//
// Created by Administrator on 2018/7/3/003.
//

#include <my_log.h>

#include "DecodeVideoThread.h"
DecodeVideoThread::DecodeVideoThread(AVFormatContext *afc , AVCodecContext  *vc  ,int videoIndex){
    maxPackage = 100;
    this->afc = afc;
    this->vc = vc;
    vframe = av_frame_alloc();
    this->videoIndex = videoIndex;
    file = fopen("sdcard/FFmpeg/fileyuv" , "wb+");
}



void DecodeVideoThread::run() {
    int result;

    while (!isExit) {
        if (pause) {
            threadSleep(50);
            continue;
        }

        if (videoPktQue.empty()) {
            LOGE(" VIDEO PACKAGE NULL ");
            threadSleep(2);
            continue;
        }
        AVPacket *pck = videoPktQue.front();
        if (pck == NULL) {
            LOGE(" video packet null !");
            videoPktQue.pop();
            continue;
        }
        //音视频同步处理
        pts = util.getConvertPts(pck->pts, afc->streams[videoIndex]->time_base);
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


        while (!isExit) {
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
            myData->isAudio = false;
            myData->vWidth = vc->width ;
            myData->vHeight = vc->height;
            int size = vc->width *  vc->height;
            myData->size = (vframe->linesize[0] + vframe->linesize[1] + vframe->linesize[2]) * vframe->height;

            //y
            myData->datas[0] = (uint8_t *)malloc(size);
            //u
            myData->datas[1] = (uint8_t *)malloc(size / 4);
            //v
            myData->datas[2] = (uint8_t *)malloc(size / 4);


            //把yuv数据读取出来
            for(int i = 0 ;i < vc->height ; ++i){
                memcpy(myData->datas[0] + vc->width * i,vframe->data[0] + vframe->linesize[0] * i ,  vc->width );
            }

            for(int i = 0 ;i < vc->height / 2 ; ++i){
                memcpy(myData->datas[1] + vc->width / 2 * i ,vframe->data[1] + vframe->linesize[1] * i ,  vc->width / 2 );
            }

            for(int i = 0 ;i < vc->height / 2 ; ++i){
                memcpy(myData->datas[2]  + vc->width / 2 * i,vframe->data[2] + vframe->linesize[2] * i ,  vc->width / 2);
            }
//            LOGE(" DECODE VIDEO ");
//            fwrite(myData->datas[0] , 1, size , file);
//            fwrite(myData->datas[1] , 1, size / 4 , file);
//            fwrite(myData->datas[2] , 1, size / 4, file);
//            threadSleep(40);
            this->notify(myData);
        }
    }
}

void DecodeVideoThread::clearQue(){
    while(!isExit){

        if(!videoPktQue.empty()){
            AVPacket *pkt = videoPktQue.front();
            if(pkt != NULL){
                av_packet_free(&pkt);
            }
            videoPktQue.pop();
            continue;
        }
        break;
    }
}



void DecodeVideoThread::update(MyData *mydata) {
    if (mydata->isAudio) return ;
//    pthread_mutex_lock(&mutex_pthread);
    while (!isExit) {
            LOGE(" VIDEO 阻塞 %d pts %lld " , videoPktQue.size() , mydata->pts);
            if (videoPktQue.size() < maxPackage) {
                videoPktQue.push(mydata->pkt);
                break;
            }
            else{
                threadSleep(2);
            }
    }
//    pthread_mutex_unlock(&mutex_pthread);
}

DecodeVideoThread::~DecodeVideoThread(){
    av_frame_free(&vframe);
}