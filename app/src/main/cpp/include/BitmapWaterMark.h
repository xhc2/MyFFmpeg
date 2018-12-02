//
// Created by Administrator on 2018/12/1/001.
//

#ifndef MYFFMPEG_BITMAPWATERMARK_H
#define MYFFMPEG_BITMAPWATERMARK_H

#include "FilterParent.h"
#include "EditParent.h"
#include <queue>

using namespace std;

class BitmapWaterMark : public FilterParent {
private :
    const char *filter_descr = "movie=%s[wm];[in][wm]overlay=%d:%d[out]";
    AVFormatContext *fmtCtx;
    AVCodecContext *decCtx;
    int videoStreamIndex;
    int audioStreamIndex;
    queue<AVPacket *> audioQue;

    //输出相关
    AVFormatContext *afcOutput;
    AVCodecContext *vCtxE;
    int buildOutput(  const char *outputPath);
public :
    BitmapWaterMark(const char *videoInputPath,  const char *outputPath, const char *logoPath, int x, int y);

    void startWaterMark();

    ~BitmapWaterMark();
};

#endif //MYFFMPEG_BITMAPWATERMARK_H
