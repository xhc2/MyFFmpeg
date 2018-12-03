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

class BitmapWaterMark : public FilterParent ,MyThread{
private :
    bool decodeFlag ;
    const char *filter_descr = "movie=%s[wm];[in][wm]overlay=%d:%d[out]";
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
public :
    BitmapWaterMark(const char *videoInputPath,  const char *outputPath, const char *logoPath, int x, int y);
    void run();
    void startWaterMark();
    void clearAllQue();
    ~BitmapWaterMark();
};

#endif //MYFFMPEG_BITMAPWATERMARK_H
