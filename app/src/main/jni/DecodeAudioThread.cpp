//
// Created by dugang on 2018/6/29.
//



#include <my_log.h>

#include "DecodeAudioThread.h"
#include "Utils.h"

void DecodeAudioThread::run() {
    int result = 0;
    while (!isExit) {
        if (pause) {
            threadSleep(50);
            continue;
        }
        if (audioFrameQue->size() >= maxFrame || audioPktQue->empty()) {
            threadSleep(2);
            continue;
        }

        AVPacket *pck = audioPktQue->front();
        audioPktQue->pop();
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
                threadSleep(2);
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
            myData.size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) *
                          aframe->nb_samples;
            myData.data = (char *) malloc(myData.size);
            memcpy(myData.data, play_audio_temp, myData.size);
            myData.isAudio = true;
            myData.pts = utils.getConvertPts(aframe->pts,
                                       afc->streams[audioIndex]->time_base);

            audioFrameQue->push(myData);
        }
    }
}

DecodeAudioThread::DecodeAudioThread(queue<MyData> *audioFrameQue , queue<AVPacket *> *audioPktQue ,
                                     AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex  , SwrContext *swc, AVFrame *aframe){
    maxFrame = 100;
    this->audioFrameQue = audioFrameQue;
    this->audioPktQue = audioPktQue;
    this->ac = ac;
    this->afc = afc;
    this->audioIndex = audioIndex;
    this->swc = swc;
    this->aframe = aframe;
    play_audio_temp = (char *)malloc(1024 * 2);



}

DecodeAudioThread::DecodeAudioThread(queue<MyData> *audioFrameQue , queue<AVPacket *> *audioPktQue ,
                                     AVCodecContext *ac ,AVFormatContext *afc  , int audioIndex  ){
    maxFrame = 100;
    this->audioFrameQue = audioFrameQue;
    this->audioPktQue = audioPktQue;
    this->ac = ac;
    this->afc = afc;
    this->audioIndex = audioIndex;
    play_audio_temp = (char *)malloc(1024 * 2);

    this->aframe = av_frame_alloc();

    swc = swr_alloc_set_opts(NULL,
                             av_get_default_channel_layout(1),
                             AV_SAMPLE_FMT_S16, ac->sample_rate,
                             av_get_default_channel_layout(ac->channels),
                             ac->sample_fmt, ac->sample_rate,
                             0, 0);
    if (swr_init(swc) < 0) {
        LOGE(" swr_init FAILD !");
    }
};

DecodeAudioThread::~DecodeAudioThread(){

}