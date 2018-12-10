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

class VideoRunBack : public MyThread , EditParent{

private :

    //output
    vector<AVPacket *> audioQue;
    vector<int64_t> keyFrameQue;
    AVFormatContext *afc_output;
    char *outPath;
    int outWidth;
    int outHeight;
    int videoIndexOutput;
    int audioIndexOutput;
    AVCodecContext *vCtxE;
    int gopCount;
    //input
    AVFormatContext *afc_input;
    char *inputPath;
    int videoIndexInput;
    int audioIndexInput;
    AVCodec *videoCodecD;
    AVCodecContext *vCtxD;
    int inWidth ;
    int inHeight;
    int64_t frameDuration ;
    char *readBuffer;
    FILE *fCache;
    int yuvSize ;
    AVFrame *outFrame ;
    int encodeFrameCount ;
    int reverseFile();
    int initInput();
    int buildOutput();
    void initValue();
    void writeFrame2File(AVFrame *frame , FILE *file);
    void clearCode(FILE *file);
public :
    VideoRunBack(const char *inputPath, const char *outPath);

    int startBackParse();
    virtual void run();
    ~VideoRunBack();
};

#endif //MYFFMPEG_VIDEORUNBACK_H
