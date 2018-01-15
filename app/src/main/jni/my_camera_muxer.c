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

int width ;
int height ;
int audio_size ;
AVOutputFormat *ofmt = NULL;
AVFormatContext *ofmt_ctx ;
int video_outindex = -1 , audio_outindex = -1;



int init_camera_muxer(const char *outputPath , int w , int h , int aSize){
    int ret = 0 ;
    width = w;
    height = h ;
    audio_size = aSize;
    av_register_all();

    ret = avformat_alloc_output_context2(&ofmt_ctx , NULL , NULL , outputPath);
    if(ret < 0){
        LOGE(" OPEN AVFormatContext FAILD ");
        return -1 ;
    }
    ofmt = ofmt_ctx->oformat;
    initMuxerVideo();
    initMuxerAudio();


    LOGE("init_camera_muxer SUCCESS %s"  , ofmt->name);
    return ret;
}


int initMuxerVideo(){
    int ret = -1 ;
    AVStream *out_stream = avformat_new_stream(ofmt_ctx , 0 );
    if (out_stream == NULL) {
        LOGE(" out_stream FAILD !");
        return -1;
    }
    out_stream->codec->codec_id = ofmt_ctx->oformat->video_codec;
    out_stream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
    out_stream->codec->width = width;
    out_stream->codec->height = height;
    out_stream->codec->bit_rate = 400000;
    //设置图像组的大小，表示两个i帧之间的间隔
    out_stream->codec->gop_size = 100;
    out_stream->codec->time_base.num = 1;
    out_stream->codec->time_base.den = 25;
    //最小视频量化标度，设定最小质量。
    out_stream->codec->qmin = 30;
    out_stream->codec->qmax = 51;

    video_outindex = out_stream->index;
    LOGE(" VIDEO_OUTINDEX %d " , video_outindex);
//    out_stream->codec->codec_tag = 0;
    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER ){
        out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    return ret ;
}

int initMuxerAudio(){
    int ret = -1 ;
    AVStream *audio_st  = avformat_new_stream(ofmt_ctx , 0);
    if (audio_st==NULL){
        LOGE(" audio_st FAILD ");
        return -1;
    }
    audio_st->codec->codec_id = ofmt_ctx->oformat->audio_codec;
    audio_st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    audio_st->codec->sample_fmt = AV_SAMPLE_FMT_S16;
    audio_st->codec->sample_rate= 44100;
    audio_st->codec->channel_layout=AV_CH_LAYOUT_STEREO;
    audio_st->codec->channels = av_get_channel_layout_nb_channels(audio_st->codec->channel_layout);
    audio_st->codec->bit_rate = 64000;
    LOGE("XHC AUDIO FORMAT NAME %s " ,audio_st->codec->codec_name );
    AVCodec *avCodec = avcodec_find_encoder(audio_st->codec->codec_id );
    if(avCodec == NULL){
        LOGE(" AUDIO CODE FAILD ");
        return -1;
    }
    LOGE(" AUDIO CODE SUCCESS %s" , avCodec->name);
    return ret ;
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