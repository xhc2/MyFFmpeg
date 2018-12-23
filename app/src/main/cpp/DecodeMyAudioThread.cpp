//
// Created by dugang on 2018/7/4.
//

#include <my_log.h>
#include "DecodeMyAudioThread.h"


void DeocdeMyAudioThread::run() {
    int result = 0;
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
        if (audioPktQue.empty()) {
//            LOGE(" AUDIO PACKAGE NULL ");
            if(finishFlag){
                //播放完毕了。
                this->notify(NULL);
            }
            threadSleep(2);
            continue;
        }
        AVPacket *pck = audioPktQue.front();
        audioPktQue.pop();
        if (!pck) {
            LOGE(" packet null !");
            continue;
        }
        result = avcodec_send_packet(ac, pck);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            av_packet_free(&pck);
            continue;
        }

        av_packet_free(&pck);
        while (true) {
            result = avcodec_receive_frame(ac, aframe);
//            LOGE(" avcodec_receive_frame ");
            if (result < 0) {
                break;
            }
            uint8_t *out[1] = {0};
            out[0] = (uint8_t *) play_audio_temp;
            MyData *myData = new MyData();
            //音频重采样 , 这里需要改下。
            swr_convert(swc, out,
                        aframe->nb_samples,
                        (const uint8_t **) aframe->data,
                        aframe->nb_samples);
            //音频部分需要自己维护一个缓冲区，通过他自己回调的方式处理
            //size = 一个sample多少个字节 * 有多少个sample。
            myData->size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) *
                           aframe->nb_samples;
            myData->data = (char *) malloc(myData->size);
            memcpy(myData->data, play_audio_temp, myData->size);
            myData->isAudio = true;
            myData->pts = utils.getConvertPts(aframe->pts,
                                              afc->streams[audioIndex]->time_base);

            this->notify(myData);
        }
    }

}

void DeocdeMyAudioThread::clearQue() {
    while (!isExit) {

        if (!audioPktQue.empty()) {
            AVPacket *pkt = audioPktQue.front();
            if (pkt != NULL) {
                av_packet_free(&pkt);
            }
            audioPktQue.pop();
            continue;
        }
        break;
    }
}

void DeocdeMyAudioThread::update(MyData *mydata) {

    if(NULL == mydata){
        //播放完毕
//        LOGE(" audio play finish ");
        finishFlag = true;
        return ;
    }
    if (!mydata->isAudio)return;
//    pthread_mutex_lock(&mutex_pthread);
    while (!isExit) {
//        LOGE(" AUDIO 阻塞 ");
        if (pause) {
            //目前有两种暂停情况。用户手动暂停视频，和用户seek暂停视频。丢掉一帧问题不大，但是可以解决掉
            //暂停时可以阻塞的情况
            break;
        }
        if (audioPktQue.size() < maxPackage) {
            audioPktQue.push(mydata->pkt);
            break;
        } else {
            threadSleep(2);
        }

    }
//    pthread_mutex_unlock(&mutex_pthread);

}

void DeocdeMyAudioThread::setQueue(queue<AVPacket *> aq) {
    for (int i = 0; i < aq.size(); ++i) {
        audioPktQue.push(aq.front());
        aq.pop();
    }
}

DeocdeMyAudioThread::DeocdeMyAudioThread(AVCodecContext *ac, AVFormatContext *afc, int audioIndex) {
    finishFlag = false;
    maxPackage = 130;
    this->ac = ac;
    this->afc = afc;
    this->audioIndex = audioIndex;
    aframe = av_frame_alloc();
    play_audio_temp = (uint8_t *) malloc(1024 * 2 * 1);
    //重采样
    swc = swr_alloc_set_opts(NULL,
                             av_get_default_channel_layout(1),
                             AV_SAMPLE_FMT_S16, ac->sample_rate,
                             av_get_default_channel_layout(ac->channels),
                             ac->sample_fmt, ac->sample_rate,
                             0, 0);
    if (swr_init(swc) < 0) {
        LOGE(" swr_init FAILD !");
    }
}

DeocdeMyAudioThread::~DeocdeMyAudioThread() {

    if (aframe != NULL) {
        av_frame_free(&aframe);
    }
    if (swc != NULL) {
        swr_free(&swc);
    }


}
