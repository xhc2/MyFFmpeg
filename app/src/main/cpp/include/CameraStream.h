//
// Created by dugang on 2018/8/2.
//

#ifndef MYFFMPEG_CAMERASTREAM_H
#define MYFFMPEG_CAMERASTREAM_H

#include <stdio.h>
#include <queue>
#include "MyThread.h"
#include "my_data.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include <libswresample/swresample.h>
};
using namespace std;
class CameraStream : public MyThread{

private :

    const char *url;
    char *yuv;
    char *pcm;
    int width, height;
    int outWidth, outHeight;
    SwsContext *sws;
    int size;
    int pcmSize;
    int pcmMinSize;
    //用来测试使用。
    FILE *filePCM;
//    FILE *fileV;
    CallJava *cj;
    AVFormatContext *afc;
    AVOutputFormat *afot;
    AVStream *videoOS;
    AVStream *audioOS;
    AVFrame *framePic;
    AVFrame *outFrame;
    AVFrame *frameAudio;
    AVPixelFormat pixFmt;
    AVCodecContext *vCodeCtx;
    AVCodecContext *aCodeCtx;
    int64_t vpts, apts;
    int64_t vCalDuration , aCalDuration;
    int nbSample;
    int64_t wvpts , wapts ;
    AVRational timeBaseFFmpeg;
    void initFFmpeg();

    void addVideoStream();

    void addAudioStream();

    void encodeVideoFrame();
    void encodeAudioFrame(int pcmSize);

    void writeVideoPacket();
    void writeAudioPacket();

    int videoIndex;
    int audioIndex;
    int count;
    bool initSuccess;
    queue<MyData *> audioPktQue;
    queue<MyData *> videoPktQue;

public :
    CameraStream(const char *url, int width, int height, int pcmsize, CallJava *cj);

    ~CameraStream();

    void pushVideoStream(jbyte *yuv);

    void pushAudioStream(jbyte *pcm, int size);

    int startRecord();

    int pauseRecord();

    void run();
};

#endif //MYFFMPEG_CAMERASTREAM_H
