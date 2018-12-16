//
// Created by Administrator on 2018/12/13/013.
//

#ifndef MYFFMPEG_VIDEODUB_H
#define MYFFMPEG_VIDEODUB_H

#include <android/native_window.h>
#include <queue>

#include "EditParent.h"
#include "MyThread.h"
#include "YuvPlayer.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
};
using namespace std;
class VideoDub : public EditParent, public MyThread, public Notify {
private :
    bool readEnd ;
    YuvPlayer *yuvPlayer;
    AVStream *inputVideoStream;
    AVFormatContext *afc_input;
    AVCodecContext *vCtxD;
    int videoStreamIndex;
    int width;
    int height;
    SwrContext *swc;

    int buildInput(const char *inputPath);


    //output

    queue<AVPacket *> audioQue;
    queue<AVPacket *> videoQue;
    int64_t apts, vpts;
    uint8_t **src_data;
    int src_linesize ;
    int audioCount ;
    int64_t  aCalDuration;
    int outChannel ;
    uint64_t outChannelLayout;
    AVSampleFormat sampleFormat ;
    AVFrame *outAFrame;
    int sampleRate;
    int nbSample ;
    AVFormatContext *afc_output;
    AVCodecContext *vCtxE;
    AVCodecContext *aCtxE;
    AVStream *videoOutputStream;
    AVStream *audioOutputStream;
    uint8_t *audioOutBuffer;
    AVAudioFifo *audioFifo;
    int audioFifoBufferSample;
    void allocAudioFifo(AVSampleFormat sample_fmt, int channels, int nb_samples) ;
    int buildOutput(const char *outputPath);
    void destroySwrContext();
    int initSwrContext(int channelCount, AVSampleFormat in_sample_fmt, int in_sample_rate);

public :
    VideoDub(const char *path, const char *outputPath, ANativeWindow *win);

    int startDub();

    int setFlag(bool flag);

    void run();

    void update(MyData *mydata);

    void addVoice(char *pcm, int size);

    ~VideoDub();

};

#endif //MYFFMPEG_VIDEODUB_H
