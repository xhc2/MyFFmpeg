//
// Created by Administrator on 2018/12/5/005.
//

#ifndef MYFFMPEG_GIFMAKE_H
#define MYFFMPEG_GIFMAKE_H

#include "EditParent.h"
extern "C"{
#include <libswscale/swscale.h>
};
class GifMake : public EditParent{
private:
    //输入相关
    AVFormatContext *afc_input;
    AVCodecContext *vCtxD ;
    int videoStreamIndex;
    //输出相关
    int outWidth ;
    int outHeight ;
    int frameCount ;
    bool isExit ;
    int64_t vCalDuration;
    AVFormatContext *afc_output ;
    AVCodecContext *vCtxE ;
    SwsContext *sws;
    AVPixelFormat outFormat ;
    int startSecond ;
    int endSecond ;
    int initSwsContext(int inWidth , int inHeight , int inpixFmt);
    void destroySwsContext();
public:
    GifMake(const char *inputPath , const char* outPath , int startSecond, int endSecond);
    int  buildInput(const char *inputPath );
    int buildOutput( const char* outPath );
    int startParse();
    ~GifMake();
};

#endif //MYFFMPEG_GIFMAKE_H
