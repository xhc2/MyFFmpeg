//
// Created by Administrator on 2018/11/6/006.
//

#ifndef MYFFMPEG_VIDEORUNBACK_H
#define MYFFMPEG_VIDEORUNBACK_H

#include "EditParent.h"
#include "MyThread.h"
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
#include <libavutil/audio_fifo.h>
}

using namespace std;

class VideoRunBack  :public MyThread{

private :

    //output
    vector<AVPacket *> audioQue;
    AVFormatContext *afc_output;
    AVOutputFormat *afot;
    char *outPath;
    int outWidth;
    int outHeight;
    int outFrameRate;
    int videoIndexOutput;
    int audioIndexOutput;
    AVCodec *videoCodecE;
    AVCodecContext *vCtxE;
    AVStream *videoOutStream;
    AVStream *audioOutStream;
    int gopCount;

    //input
    AVFormatContext *afc_input;
    int gopSize;
    char *inputPath;
    int videoIndexInput;
    int audioIndexInput;
    AVCodec *videoCodecD;
    AVCodecContext *vCtxD;
    AVStream *videoinputStream;
    AVStream *audioinputStream;
    int inWidth ;
    int inHeight;
    int64_t inputDuration;
    int64_t gopDuration ;

    int initInput();
    void initValue();

    int initOutput();

    int addVideoOutputStream(int width, int height);

    int addAudioOutputStream();

public :
    VideoRunBack(const char *inputPath, const char *outPath);

    void startBackParse();
    virtual void run();
    ~VideoRunBack();
};

#endif //MYFFMPEG_VIDEORUNBACK_H
