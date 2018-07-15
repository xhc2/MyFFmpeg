//
// Created by Administrator on 2018/7/3/003.
//

#include <my_log.h>

#include "DecodeVideoThread.h"
int count ;
DecodeVideoThread::DecodeVideoThread(AVFormatContext *afc , AVCodecContext  *vc  ,int videoIndex){
    maxPackage = 100;
    this->afc = afc;
    this->vc = vc;
    count = 0;
    vframe = av_frame_alloc();
    this->videoIndex = videoIndex;
    fileYuv = fopen("sdcard/FFmpeg/fileyuv" , "wb+");
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
            myData->isAudio = false;
            myData->vWidth = vc->width ;
            myData->vHeight = vc->height;
            int size = vc->width *  vc->height;
            LOGE(" VFRAME HEIGHT %d "  , vframe->height);
            myData->size = (vframe->linesize[0] + vframe->linesize[1] + vframe->linesize[2]) * vframe->height;

            fwrite(vframe->data[0] , 1 , vframe->linesize[0] * vc->height, fileYuv);
            fwrite(vframe->data[1], 1, vframe->linesize[1] * vc->height / 4, fileYuv);
            fwrite(vframe->data[2], 1, vframe->linesize[2] * vc->height / 4, fileYuv);

            //y
            myData->datas[0] = (uint8_t *)malloc(size);
            //u
            myData->datas[1] = (uint8_t *)malloc(size / 4);
            //v
            myData->datas[2] = (uint8_t *)malloc(size / 4);

            memcpy(myData->datas[0] ,vframe->data[0] , size );
            memcpy(myData->datas[1] ,vframe->data[1] , size / 4 );
            memcpy(myData->datas[2] ,vframe->data[2] , size / 4);
            for(int i = 0 ;i < 8 ; ++ i){
                if(vframe->linesize[i] != 0){
                    myData->linesize[i] = vframe->linesize[i];
                }
            }

            av_image_copy(video_dst_data, video_dst_linesize,
                          (const uint8_t **)(frame->data), frame->linesize,
                          pix_fmt, width, height);

//            LOGE(" LINE SIZE %d , w * h %d " , vframe->linesize[0] * myData->vHeight , size);

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
    av_frame_free(&vframe);
}