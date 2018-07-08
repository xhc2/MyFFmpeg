//
// Created by dugang on 2018/7/4.
//

#include <my_log.h>
#include "DecodeMyAudioThread.h"


void DeocdeMyAudioThread::run(){
    int result = 0;
    while(!isExit){
        if(pause){
            threadSleep(2);
            continue;
        }
        if(audioPktQue.empty()){
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

            if (result < 0) {
                break;
            }
            uint8_t *out[1] = {0};
            out[0] = (uint8_t *) play_audio_temp;
            MyData myData;
            //音频重采样
            swr_convert(swc, out,
                        aframe->nb_samples,
                        (const uint8_t **) aframe->data,
                        aframe->nb_samples);
            //音频部分需要自己维护一个缓冲区，通过他自己回调的方式处理
            //size = 一个sample多少个字节 * 有多少个sample。
            myData.size = av_get_bytes_per_sample( AV_SAMPLE_FMT_S16) *
                          aframe->nb_samples;
            myData.data = (char *) malloc(myData.size);
            memcpy(myData.data, play_audio_temp, myData.size);
            myData.isAudio = true;
            myData.pts = utils.getConvertPts(aframe->pts,
                                       afc->streams[audioIndex]->time_base);
            LOGE("DECODE AUDIO");
            this->notify(myData);
        }
    }

}

void DeocdeMyAudioThread::update(MyData mydata){
    if(!mydata.isAudio)return ;
    while(true){

            pthread_mutex_lock(&mutex_pthread);
//            LOGE(" audioPktQue.size() %d " , audioPktQue.size());
            if(audioPktQue.size() < maxPackage){
                audioPktQue.push(mydata.pkt);
                pthread_mutex_unlock(&mutex_pthread);
                break;
            }
            else{
                threadSleep(2);
                pthread_mutex_unlock(&mutex_pthread);
            }

    }


}

DeocdeMyAudioThread::DeocdeMyAudioThread( AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex  ){
    maxPackage = 130;
    this->ac = ac;
    this->afc = afc;
    this->audioIndex = audioIndex;
    aframe = av_frame_alloc();
    play_audio_temp = (uint8_t *)malloc(1024 * 2 * 1);
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

DeocdeMyAudioThread::~DeocdeMyAudioThread(){

}
