//
// Created by Administrator on 2018/12/1/001.
//

#ifndef MYFFMPEG_BITMAPWATERMARK_H
#define MYFFMPEG_BITMAPWATERMARK_H

#include "FilterParent.h"
#include "EditParent.h"
#include "MyThread.h"
#include <queue>

using namespace std;

class VideoFilter : public FilterParent ,MyThread{
private :
    bool decodeFlag ;

    AVFormatContext *fmtCtx;
    AVCodecContext *decCtx;
    int videoStreamIndex;
    int audioStreamIndex;

    //输出相关
    int64_t vpts ;
    int64_t apts ;
    queue<AVPacket *> audioQue;
    queue<AVPacket *> videoQue;
    AVFormatContext *afcOutput;
    AVCodecContext *vCtxE;
    bool readEnd;
    int buildOutput(  const char *outputPath);
    int64_t duration ;
    int *paramsSet;
    int paramsSetSize;
public :
    VideoFilter(const char *videoInputPath,  const char *outputPath, const char *filter_descr  ,int* params , int paramsSize);
    void run();
    void startWaterMark();
    void clearAllQue();
    ~VideoFilter();
};

#endif //MYFFMPEG_BITMAPWATERMARK_H
