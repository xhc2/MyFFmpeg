//
// Created by dugang on 2018/8/2.
//

#ifndef MYFFMPEG_CAMERASTREAM_H
#define MYFFMPEG_CAMERASTREAM_H

#include <stdio.h>
#include <queue>

#include "MyThread.h"
#include "my_data.h"
#include "EditParent.h"
extern "C"
{
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include <libavutil/audio_fifo.h>

};
using namespace std;
class CameraStream : public MyThread , public EditParent{
private :
    queue<AVPacket *> audioPktQue;
    queue<AVPacket *> videoPktQue;
    AVFormatContext *afc_output;
    AVStream *audioStream ;
    AVStream *videoStream;
    AVCodecContext *aCtxE;
    AVCodecContext *vCtxE;
    CallJava *callJava ;
    int inputWidth ;
    int inputHeight;
    int outputWidth ;
    int outputHeight;
    int outSampleRate ;
    AVSampleFormat outSampleForamt;
    int nbSample;
    int64_t vCalDuration ;
    int64_t aCalDuration ;
    int64_t apts , vpts ;
    int64_t aCount , vCount;
    int64_t dropVCount , dropACount ;
    SwsContext *sws;
    AVFrame *framePic ;
    AVFrame *frameOutV ;
    AVFrame *frameOutA ;
    AVAudioFifo *fifo;
    bool initSuccess;
    void destroyAudioFifo();
    void allocAudioFifo(AVSampleFormat sample_fmt, int channels, int nb_samples);
    int initSwsContext(int inWidth, int inHeight, int inpixFmt)  ;
    void destroySwsContext() ;
public :
    CameraStream();
    int init(const char *url, int width, int height, int pcmsize, CallJava *cj);
    ~CameraStream();

    void pushVideoStream(jbyte *yuv);

    void pushAudioStream(jbyte *pcm, int size);

    int startRecord();

    int pauseRecord();

    void run();
};

#endif //MYFFMPEG_CAMERASTREAM_H
