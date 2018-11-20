//
// Created by Administrator on 2018/11/20/020.
//

#ifndef MYFFMPEG_CURRENTTIMEBITMAP_H
#define MYFFMPEG_CURRENTTIMEBITMAP_H

#include "my_log.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/jni.h>
}

class CurrentTimeBitmap {

private :

    AVFormatContext *afc;
    AVCodecContext *vc;
    int video_index;
    AVCodec *videoCode;
    char *path ;
    int outWidth ;
    int outHeight;
    AVFrame *outVFrame;
    SwsContext *sws;
    int initFFmpeg(const char *path);
    AVFrame *deocdePacket(AVPacket *packet);
    int initSwsContext(int inWidth, int inHeight, int inpixFmt);
public:

    char* getCurrentBitmap(float time);

    CurrentTimeBitmap(const char* path , int outWidth , int outHeight);

    ~CurrentTimeBitmap();
};

#endif //MYFFMPEG_CURRENTTIMEBITMAP_H
