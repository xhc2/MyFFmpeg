//
// Created by dugang on 2018/8/2.
//

#ifndef MYFFMPEG_CAMERASTREAM_H
#define MYFFMPEG_CAMERASTREAM_H

#include <stdio.h>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"

};
class CameraStream{

private :

    const char *url;
    char *yuv;
    int width , height;
    int size ;
    //用来测试使用。
    FILE *file;
    CallJava *cj ;
    AVFormatContext *afc;
    AVOutputFormat *afot;
    AVStream *os;
    AVFrame *framePic;
    AVPixelFormat pixFmt;
    AVCodecContext *vCodeCtx;
    int count   ;
public :
    CameraStream(const char * url , int width , int height , CallJava *cj);
    ~CameraStream();
    void initFFmpeg();
    void pushStream(jbyte *yuv);
};

#endif //MYFFMPEG_CAMERASTREAM_H
