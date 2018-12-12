//
// Created by Administrator on 2018/11/6/006.
//

#ifndef MYFFMPEG_VIDEORUNBACK_H
#define MYFFMPEG_VIDEORUNBACK_H

#include "EditParent.h"
#include "MyThread.h"
#include <queue>
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
#include <libavutil/audio_fifo.h>
}

using namespace std;

class VideoRunBack :   public EditParent  , MyThread{

private :

    //output
    vector<int64_t> keyFrameQue;
    queue<AVPacket *> queVideo;
    queue<AVPacket *> queAudio;
    AVFormatContext *afc_output;
    char *outPath;
    int outWidth;
    int outHeight;
    AVCodecContext *vCtxE;
    AVCodecContext *aCtxE;
    int gopCount;
    bool dealEnd;
    int64_t videoStreamDuration;
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
    int64_t vpts ;
    int64_t apts ;
    int nowKeyFramePosition ;

    int seekLastKeyFrame();
    int reverseFile();
    int initInput();
    int buildOutput();
    void initValue();
    void writeFrame2File(AVFrame *frame , FILE *file);
    void clearCode(FILE *file);
    void run();
    //release
    void destroyInput();
    void destroyOutput();
    void destroyOther();
public :
    VideoRunBack(const char *inputPath, const char *outPath);

    int startBackParse();
    ~VideoRunBack();
};

#endif //MYFFMPEG_VIDEORUNBACK_H
