//
// Created by Administrator on 2018/11/6/006.
//

#ifndef MYFFMPEG_VIDEORUNBACK_H
#define MYFFMPEG_VIDEORUNBACK_H

#include "EditParent.h"
#include "MyThread.h"
#include <vector>
#include <queue>
#include <stack>
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
#include <libavutil/audio_fifo.h>
}

using namespace std;

class VideoRunBack :   public EditParent{

private :

    //output
    vector<int64_t> keyFrameQue;
    queue<AVPacket *> queVideo;
    stack<AVPacket *> audioStack;
    AVFormatContext *afc_output;
    char *outPath;
    int outWidth;
    int outHeight;
//    int videoIndexOutput;
//    int audioIndexOutput;
    AVCodecContext *vCtxE;
    AVCodecContext *aCtxE;
    int gopCount;


    //input
    AVFormatContext *afc_input;
    char *inputPath;
    int videoIndexInput;
    int audioIndexInput;

    AVCodecContext *vCtxD;
    AVCodecContext *aCtxD;
    int inWidth ;
    int inHeight;
    int64_t videoFrameDuration ;
    int64_t audioFrameDuration ;

    char *readBuffer;
    FILE *fCache;
    int yuvSize ;
    int ySize ;
    AVFrame *outFrame ;
    int encodeFrameVideoCount;
    int encodeFrameAudioCount;
    int64_t vpts ;
    int64_t apts ;
    int nowKeyFramePosition ;

    int test();
    int seekLastKeyFrame();
    int reverseFile();
    int initInput();
    int buildOutput();
    void initValue();
    void writeFrame2File(AVFrame *frame , FILE *file);
    void clearCode(FILE *file);
public :
    VideoRunBack(const char *inputPath, const char *outPath);

    int startBackParse();
    ~VideoRunBack();
};

#endif //MYFFMPEG_VIDEORUNBACK_H
