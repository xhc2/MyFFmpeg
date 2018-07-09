//
// Created by dugang on 2018/7/2.
//

#include <SonicRead.h>
#include <my_log.h>

SonicRead::SonicRead(int samplerate, int channel, float speed, queue<MyData> *audioFrameQue) {
    tempoStream = sonicCreateStream(samplerate, channel);
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


int SonicRead::dealAudio(short **getBuf) {

    while (!isExit) {
        if (audioFrameQue->empty()) {
            sonicFlush();
        } else {
            MyData myData = audioFrameQue->front();
            audioFrameQue->pop();
            int size = myData.size;
            if (size > putBufferSize) {
                playAudioBuffer = (short *) realloc(playAudioBuffer, size);
                putBufferSize = size;
            }
            memcpy(playAudioBuffer, myData.data, size);
            putSample(playAudioBuffer, size);
            free(myData.data);
        }

        int availiableByte = availableBytes();
        if (availiableByte > 0) {
            if (availiableByte > putBufferSize) {
                //重新分配空间大小
                getAudioBuffer = (short *) realloc(getAudioBuffer, availiableByte);
                getBufferSize = availiableByte;
            }
            int samplesReadBytes = reciveSample(getAudioBuffer, availiableByte);
            LOGE(" samplesReadBytes %d ", samplesReadBytes);
            if(samplesReadBytes > 0){
                *getBuf = getAudioBuffer;
                return samplesReadBytes;
            }
        }
    }

//    LOGE(" audioFrameQue->size %d  "  , audioFrameQue->size());
//    if (audioFrameQue->empty()) {
//
//        return 0;
//    }
//    MyData myData = audioFrameQue->front();
//    audioFrameQue->pop();
//    int size = myData.size;
//    do {
//
//        memcpy(playAudioBuffer, myData.data, myData.size);
//
//
//        int numSamples = size / (sizeof(short) * sonicGetNumChannels(tempoStream));
//        LOGE(" PUT SAMPLE %d ", numSamples);
//        sonicWriteShortToStream(tempoStream, playAudioBuffer, numSamples);
//        int availableSize = sonicSamplesAvailable(tempoStream) * sizeof(short) *
//                        sonicGetNumChannels(tempoStream);
//        LOGE(" available SAMPLE %d ", availableSize);
//        if (availableSize > 0) {
//            getAudioBuffer = (short *) malloc(availableSize);
//
//            int getSample = sonicReadShortFromStream(tempoStream, getAudioBuffer,
//                                                     availableSize / (sizeof(short) *
//                                                                  sonicGetNumChannels(
//                                                                          tempoStream)));
//            *getBuf = getAudioBuffer;
//            LOGE("getSample = %d , size = %d " , getSample , (getSample * (sizeof(short) *   sonicGetNumChannels(tempoStream))));
//            return getSample * (sizeof(short) * sonicGetNumChannels(tempoStream));
//        }
//        free(myData.data);
//    } while (size > 0);
    return 0;
}

void SonicRead::putSample(short *buf, int lenBytes) {
    int samples = lenBytes / (sizeof(short) * sonicGetNumChannels(tempoStream));
    LOGE(" putSample %d ", samples);
    sonicWriteShortToStream(tempoStream, buf, samples);
}

void SonicRead::sonicFlush() {
    sonicFlushStream(tempoStream);
}

int SonicRead::availableBytes() {

    return sonicSamplesAvailable(tempoStream) * sizeof(short) * sonicGetNumChannels(tempoStream);
}

int SonicRead::reciveSample(short *getBuf, int lenByte) {
    int available =
            sonicSamplesAvailable(tempoStream) * sizeof(short) * sonicGetNumChannels(tempoStream);

    if (lenByte > available) {
        LOGE(" reciveSample  lenByte > available ");
        lenByte = available;
    }

    int samplesRead = sonicReadShortFromStream(tempoStream, getBuf,
                                               lenByte /
                                               (sizeof(short) * sonicGetNumChannels(tempoStream)));
    LOGE("reciveSample samplesRead %d ", samplesRead);
    int bytesRead = samplesRead * sizeof(short) * sonicGetNumChannels(tempoStream);
    return bytesRead;
}

SonicRead::~SonicRead() {

}