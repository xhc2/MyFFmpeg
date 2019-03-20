//
// Created by dugang on 2018/7/2.
//

#include <SonicRead.h>
#include <my_log.h>
#include "unistd.h"

SonicRead::SonicRead(int samplerate, int channel, float speed, queue<MyData *> *audioFrameQue ) {
    tempoStream = sonicCreateStream(samplerate, channel);
    this->sampleRate = samplerate ;
    this->channel = channel;
    sonicSetSpeed(tempoStream, speed);
    sonicSetPitch(tempoStream, 1.0);
    sonicSetRate(tempoStream, 1.0);
    this->audioFrameQue = audioFrameQue;
    putBufferSize = 1024 * 2 * 2;
    getBufferSize = 1024 * 2 * 2;
    playAudioBuffer = (short *) malloc(putBufferSize);
    getAudioBuffer = (short *) malloc(getBufferSize);
    isExit = false;
}

void SonicRead::changeSpeed(float speed){
    sonicSetSpeed(tempoStream, speed);
}

int SonicRead::dealAudio(short **getBuf , int64_t &pts) {
    while (!isExit) {
        if (audioFrameQue->empty()) {
            if(sonicFlush() <= 0){
                break;
            };
        } else {
            MyData *myData = audioFrameQue->front();
            audioFrameQue->pop();
            if(myData == NULL || myData->data == NULL){
                break;
            }
            pts = myData->pts;
            int size = myData->size;
            if (size > putBufferSize) {
                playAudioBuffer = (short *) realloc(playAudioBuffer, size);
                putBufferSize = size;
            }
            memcpy(playAudioBuffer, myData->data, size);
            putSample(playAudioBuffer, size);
            delete myData;
        }

        int availiableByte = availableBytes();
        if (availiableByte > 0) {
            if (availiableByte > putBufferSize) {
                //重新分配空间大小
                getAudioBuffer = (short *) realloc(getAudioBuffer, availiableByte);
                getBufferSize = availiableByte;
            }
            int samplesReadBytes = reciveSample(getAudioBuffer, availiableByte);
            if(samplesReadBytes > 0){
                *getBuf = getAudioBuffer;
                return samplesReadBytes;
            }
        }
    }
    return 0;
}

void SonicRead::putSample(short *buf, int lenBytes) {
    int samples = lenBytes / (sizeof(short) * sonicGetNumChannels(tempoStream));
    sonicWriteShortToStream(tempoStream, buf, samples);
}

int SonicRead::sonicFlush() {
   return sonicFlushStream(tempoStream);
}

int SonicRead::availableBytes() {

    return sonicSamplesAvailable(tempoStream) * sizeof(short) * sonicGetNumChannels(tempoStream);
}

int SonicRead::reciveSample(short *getBuf, int lenByte) {
    int available =
            sonicSamplesAvailable(tempoStream) * sizeof(short) * sonicGetNumChannels(tempoStream);

    if (lenByte > available) {
        lenByte = available;
    }

    int samplesRead = sonicReadShortFromStream(tempoStream, getBuf,
                                               lenByte /
                                               (sizeof(short) * sonicGetNumChannels(tempoStream)));
    int bytesRead = samplesRead * sizeof(short) * sonicGetNumChannels(tempoStream);
    return bytesRead;
}

//void SonicRead::destroySonicRead(){
//    sonicDestroyStream(tempoStream);
//    tempoStream = NULL;
//}
//void SonicRead::createSonicRead(){
//    tempoStream = sonicCreateStream(sampleRate, channel);
//    LOGE(" SONIC CREATE SUCCESS ? %d " , (tempoStream == NULL));
//    sonicSetSpeed(tempoStream, 1.0);
//    sonicSetPitch(tempoStream, 1.0);
//    sonicSetRate(tempoStream, 1.0);
//}

SonicRead::~SonicRead() {
    isExit = true;
    if(playAudioBuffer != NULL){
        free(playAudioBuffer);
    }
    if(getAudioBuffer != NULL){
        free(getAudioBuffer);
    }
    sonicDestroyStream(tempoStream);
    tempoStream = NULL;
}