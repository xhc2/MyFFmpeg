//
// Created by Administrator on 2018/10/25/025.
//

#ifndef MYFFMPEG_VIDEOJOINT_H
#define MYFFMPEG_VIDEOJOINT_H

#include <string.h>
#include <jni.h>
#include <my_log.h>
#include <vector>
#include <queue>
#include "EditParent.h"
#include "MyThread.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
#include <libavutil/audio_fifo.h>
}

using namespace std;

class VideoJoint : public MyThread {

private :
    char *outPath;
    vector <char *> inputPaths;

    int videoIndexInput;
    int audioIndexInput;

    int videoIndexOutput;
    int audioIndexOutput;

    AVCodec *videoCodecD;
    AVCodec *videoCodecE;

    AVCodec *audioCodecE;
    AVCodec *audioCodecD;

    AVCodecContext *aCtxD;
    AVCodecContext *aCtxE;

    AVCodecContext *vCtxD;
    AVCodecContext *vCtxE;

    AVFormatContext *afc_input;
    AVFormatContext *afc_output;
    AVOutputFormat *afot;

    AVStream *audioOutStream ;
    AVStream *videoOutStream ;

    int outWidth ;
    int outHeight;
    int outFrameRate ;

    SwsContext *sws;
    SwrContext *swc;

    AVFrame *outVFrame ;
    AVFrame *outAFrame ;

    int audioSampleCount ;
    int videoFrameCount ;
    int64_t vCalDuration , aCalDuration;

    bool readEnd ;

    int sampleRate ;
    AVSampleFormat sampleFormat ;
    uint64_t outChannelLayout ;
    int channel ;
    int nbSample ;

    int64_t apts , vpts;

    uint8_t *audioOutBuffer;
    AVRational timeBaseFFmpeg;

    queue<AVPacket *> audioQue;
    queue<AVPacket *> videoQue;

    int audioQueMax ;
    int videoQueMax ;

    AVAudioFifo *audioFifo;

    int initSwsContext(int inWidth , int inHeight , int inpixFmt);
    void destroySwsContext();

    int initSwrContext(int channelCount ,  AVSampleFormat  in_sample_fmt, int  in_sample_rate);
    void destroySwrContext();

    void initValue();
    void destroyAudioFifo();
    void allocAudioFifo(AVSampleFormat sample_fmt  ,  int channels, int nb_samples);

    int addVideoOutputStream(int width , int height);
    int addAudioOutputStream();
    AVPacket *encodeFrame(AVFrame *frame , AVCodecContext *encode);
    AVFrame *deocdePacket(AVPacket *packet , AVCodecContext *decode );
    void startDecode();
    AVFrame *getAudioFrame(uint8_t *data , int size);

public :
    VideoJoint(vector <char *> inputPath ,   const char *output , int outWidth , int outHeight);

    void startJoint();

    int initInput(char *path);

    void destroyInput();

    void destroyOutput();

    int initOutput(char *path);

    ~VideoJoint();


    virtual void run();
};

#endif //MYFFMPEG_VIDEOJOINT_H
