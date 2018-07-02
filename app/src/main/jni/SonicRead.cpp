//
// Created by dugang on 2018/7/2.
//

#include <SonicRead.h>
#include <my_log.h>

SonicRead::SonicRead(int samplerate, int channel, float speed, queue<MyData> *audioFrameQue) {
    tempoStream = sonicCreateStream(samplerate, channel);
    sonicSetSpeed(tempoStream, speed);
    this->audioFrameQue = audioFrameQue;
    playAudioBuffer = (short *) malloc(1024 * 2 * 2);
    getAudioBuffer = (short *) malloc(1024 * 2 * 2);
}


int SonicRead::dealAudio(short **getBuf) {
    LOGE(" audioFrameQue->size %d  "  , audioFrameQue->size());
    if (audioFrameQue->empty()) {

        return 0;
    }
    MyData myData = audioFrameQue->front();
    audioFrameQue->pop();
    int size = myData.size;
    do {

        memcpy(playAudioBuffer, myData.data, myData.size);

        free(myData.data);
        int numSamples = size / (sizeof(short) * sonicGetNumChannels(tempoStream));
        LOGE(" PUT SAMPLE %d ", numSamples);
        sonicWriteShortToStream(tempoStream, playAudioBuffer, numSamples);
        int availableSize = sonicSamplesAvailable(tempoStream) * sizeof(short) *
                        sonicGetNumChannels(tempoStream);
        LOGE(" available SAMPLE %d ", availableSize);
        if (availableSize > 0) {
            int getSample = sonicReadShortFromStream(tempoStream, getAudioBuffer,
                                                     availableSize / (sizeof(short) *
                                                                  sonicGetNumChannels(
                                                                          tempoStream)));
            *getBuf = getAudioBuffer;
            LOGE("getSample = %d , size = %d " , getSample , (getSample * (sizeof(short) *   sonicGetNumChannels(tempoStream))));
            return getSample * (sizeof(short) * sonicGetNumChannels(tempoStream));
        }
    } while (size > 0);
    return 1;
}

SonicRead::~SonicRead() {

}