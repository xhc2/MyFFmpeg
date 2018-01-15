//
// Created by dugang on 2018/1/15.
//

#include "my_camera_muxer.h"
#include "My_LOG.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include "libavutil/log.h"
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
//jstring joutputPath , jint width , jint height , jint aSize
int init_camera_muxer(const char *outputPath , int width , int height , int aSize){
    int ret = 0 ;
    LOGE("init_camera_muxer");
    return ret;
}

int encodeCamera_muxer(jbyte *nativeYuv){
    int ret = 0 ;
    LOGE("encodeCamera_muxer");
    return ret;
}

int encodeAudio_muxer(jbyte *nativePcm){
    int ret = 0 ;
    LOGE("encodeAudio_muxer");
    return ret;
}

int close_muxer(){
    LOGE("CLOSE_CAMERA_MUXER");
    return 1;
}