//
// Created by dugang on 2018/7/31.
//

#ifndef MYFFMPEG_PUBLISHSTREAM_H
#define MYFFMPEG_PUBLISHSTREAM_H
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
};

class PublishStream{

private :
    const char* url ;
    const char* inpath ;
    AVOutputFormat *ofmt ;
    AVFormatContext *ifmtCtx , *ofmtCtx;
    AVPacket *pkt;
    bool isExist ;
    int audioIndex;
    int videoIndex;

public :
    PublishStream(const char* url , const char* inpath);
    ~PublishStream();
};

#endif //MYFFMPEG_PUBLISHSTREAM_H
