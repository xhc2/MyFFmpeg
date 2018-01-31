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
#include <libswresample/swresample.h>
//http://blog.csdn.net/bixinwei22/article/details/78779259

AVOutputFormat *ofmt = NULL;
const char *mp4_output_path;
int width , height ,aSize;
AVFormatContext *ofmt_ctx ;
int init_camera_muxer(const char *outputPath , int w , int h , int aSize){
    int ret = 0;
    mp4_output_path = outputPath;
    width = w ;
    height = h;
    av_register_all();
    av_log_set_callback(custom_log);
    ofmt = av_guess_format(NULL , outputPath , NULL);
    ret = avformat_alloc_output_context2(&ofmt_ctx , NULL , NULL , outputPath);
    if(ret < 0){
        LOGE("AVFormatContext ALLOC CONTEXT FAILD !");
        return -1;
    }
    if(ofmt == NULL){
        LOGE("AVOutputFormat FAILD!");
        return -1;
    }






    LOGE(" INIT SUCCESS ！");

    return ret;
}


int encodeCamera_muxer(jbyte *nativeYuv){
    return -1;
}


int encodeAudio_muxer(jbyte *nativePcm){
    return -1;
}

int close_muxer(){
    return -1;
}

int initMuxerVideo(){
    return 1;
}


int initMuxerAudio(){
    return 1;
}
int init_muxer_Sws(){
    return -1;
}
int encodePcm_(jbyte *nativePcm){
    return 1;
}


int encodeYuv_(jbyte *nativeYuv){
    return -1;
}
int interleaved_write(AVPacket *yuvPkt , AVPacket *pcmPkt){
    return -1;
}










