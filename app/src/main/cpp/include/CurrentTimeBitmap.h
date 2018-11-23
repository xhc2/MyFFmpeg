//
// Created by Administrator on 2018/11/20/020.
//

#ifndef MYFFMPEG_CURRENTTIMEBITMAP_H
#define MYFFMPEG_CURRENTTIMEBITMAP_H

#include "my_log.h"
#include "EditParent.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavcodec/jni.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

typedef struct {
    long imageSize;
    long blank;
    long startPosition;
} BmpHead;

typedef struct {
    long Length;
    long width;
    long height;
    unsigned short colorPlane;
    unsigned short bitColor;
    long zipFormat;
    long realSize;
    long xPels;
    long yPels;
    long colorUse;
    long colorImportant;
} InfoHead;

class CurrentTimeBitmap {

private :

    AVFormatContext *afc;
    AVCodecContext *vc;
    int video_index;
    AVCodec *videoCode;
    char *path ;
    int outWidth ;
    int outHeight;
    AVPixelFormat outPix;
//    AVFrame *outVFrame;
    uint8_t *outRgbdata[4];
    int outRgbLineSize[4];

    SwsContext *sws;
    int initFFmpeg(const char *path);
    AVFrame *deocdePacket(AVPacket *packet);
    int initSwsContext(int inWidth, int inHeight, int inpixFmt);
    void yuv2Bitmap(/*uint8_t *yuv , int sWidth , int sHeight , int dWidth , int dHeight*/);
    uint8_t *yuv2Bmp(AVFrame *pFrameRGB, int width, int height, int *bmpSize );
    uint8_t * yuv2Bmp(uint8_t *rgb24_buffer, int width, int height , int *bmpSize);
public:

    void getCurrentBitmap(float time , uint8_t *bufferResult);
    void getCurrentBitmapKeyFrame(float time , uint8_t *bufferResult);

    CurrentTimeBitmap(const char* path , int outWidth , int outHeight);

    ~CurrentTimeBitmap();
};

#endif //MYFFMPEG_CURRENTTIMEBITMAP_H
