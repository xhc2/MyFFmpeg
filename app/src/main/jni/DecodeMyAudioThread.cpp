//
// Created by dugang on 2018/7/4.
//

#include <my_log.h>
#include "DecodeMyAudioThread.h"


void DeocdeMyAudioThread::run(){

}

void DeocdeMyAudioThread::update(MyData mydata){
    if(!mydata.isAudio)return ;
    while(true){

            pthread_mutex_lock(&mutex_pthread);
            LOGE(" audioPktQue.size() %d " , audioPktQue.size());
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

    this->ac = ac;
    this->afc = afc;
    this->audioIndex = audioIndex;
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

