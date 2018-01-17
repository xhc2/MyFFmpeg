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

int width ;
int height ;
int audio_size ;
AVOutputFormat *ofmt = NULL;
AVFormatContext *ofmt_ctx ;
int video_outindex = -1 , audio_outindex = -1;
//保存码流的pts
int64_t video_frame_count = 0 , audio_frame_count = 0;
AVBitStreamFilterContext *h264bsfc;
AVBitStreamFilterContext *aacbsfc ;
AVFrame *videoFrame , *audioFrame;
AVStream *video_stream;
AVStream *audio_stream;
int y_size = 0;

int init_camera_muxer(const char *outputPath , int w , int h , int aSize){
    int ret = 0 ;
    width = w;
    height = h ;
    y_size = width * height ;
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
    /**
   * AVFMT_NOFILE需要一个没有打开的File
   */
    if(!(ofmt->flags & AVFMT_NOFILE)){
        if(avio_open(&ofmt_ctx->pb , outputPath , AVIO_FLAG_WRITE) < 0){
            LOGE("OPEN OUTPUT FILE FAILD !");
            return -1;
        }
    }
    if(avformat_write_header(ofmt_ctx , NULL) < 0){
        LOGE("write_header faild ");
        return -1;
    }
    h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
    LOGE("init_camera_muxer SUCCESS %s"  , ofmt->name);




    return ret;
}


int initMuxerVideo(){
    int ret = -1 ;
    video_stream = avformat_new_stream(ofmt_ctx , 0 );
    if (video_stream == NULL) {
        LOGE(" out_stream FAILD !");
        return -1;
    }
    video_stream->codec->codec_id = ofmt_ctx->oformat->video_codec;
    video_stream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    video_stream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
    video_stream->codec->width = width;
    video_stream->codec->height = height;
    video_stream->codec->bit_rate = 400000;
    //设置图像组的大小，表示两个i帧之间的间隔
    video_stream->codec->gop_size = 100;
    video_stream->codec->time_base.num = 1;
    video_stream->codec->time_base.den = 25;
    //最小视频量化标度，设定最小质量。
    video_stream->codec->qmin = 30;
    video_stream->codec->qmax = 51;

    video_outindex = video_stream->index;
    LOGE(" VIDEO_OUTINDEX %d " , video_outindex);
    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER ){
        video_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    LOGE("VIDEO TIME BASE %f  , den %d , num %d" , av_q2d(video_stream->codec->time_base ) ,
         video_stream->codec->time_base.den , video_stream->codec->time_base.num);

    videoFrame = av_frame_alloc();
    int pic_size = avpicture_get_size(video_stream->codec->pix_fmt , video_stream->codec->width , video_stream->codec->height);
    videoFrame->format = video_stream->codec->pix_fmt;
    videoFrame->width = video_stream->codec->width;
    videoFrame->height = video_stream->codec->height;
    uint8_t *picture_buf = (uint8_t *) av_malloc(pic_size);

    /**
        * Setup the data pointers and linesizes based on the specified image parameters and the provided array.
        */
    avpicture_fill((AVPicture *) videoFrame, picture_buf, video_stream->codec->pix_fmt,
                   video_stream->codec->width, video_stream->codec->height);

    return ret ;
}
//http://ffmpeg.org/doxygen/3.2/structAVFrame.html#details
int initMuxerAudio(){
    int ret = -1 ;
    audio_stream  = avformat_new_stream(ofmt_ctx , 0);
    if (audio_stream==NULL){
        LOGE(" audio_st FAILD ");
        return -1;
    }
    audio_stream->codec->codec_id = ofmt_ctx->oformat->audio_codec;
    audio_stream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    audio_stream->codec->sample_fmt = AV_SAMPLE_FMT_S16;
    audio_stream->codec->sample_rate= 44100;
    audio_stream->codec->channel_layout=AV_CH_LAYOUT_STEREO;
    audio_stream->codec->channels = av_get_channel_layout_nb_channels(audio_stream->codec->channel_layout);
    audio_stream->codec->bit_rate = 64000;
    audio_outindex = audio_stream->index;
    LOGE("XHC AUDIO FORMAT NAME %s " ,audio_stream->codec->codec_name );
    AVCodec *avCodec = avcodec_find_encoder(audio_stream->codec->codec_id );
    if(avCodec == NULL){
        LOGE(" AUDIO CODE FAILD ");
        return -1;
    }
    LOGE(" AUDIO CODE SUCCESS %s" , avCodec->name);
    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
        audio_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    LOGE("AUDIO TIME BASE %f   , den %d , num %d\"" , av_q2d(audio_stream->codec->time_base )
    , audio_stream->codec->time_base.den , audio_stream->codec->time_base.num);

    audioFrame = av_frame_alloc();
    audioFrame->format = audio_stream->codec->sample_fmt;

    return ret ;
}

int encodeYuv(jbyte *nativeYuv){
    videoFrame->data[0] = (uint8_t *)nativeYuv;
    videoFrame->data[1] = (uint8_t *) nativeYuv + y_size;
    videoFrame->data[2] =(uint8_t *) nativeYuv + y_size * 5 / 4;
    videoFrame->pts = video_frame_count * (video_stream->time_base.den) / ((video_stream->time_base.num) * 25);



    return 1;
}

int encodePcm(jbyte *nativePcm){

    return 1;
}

int encode(jbyte *nativeYuv , jbyte *nativePcm){
    if(av_compare_ts(video_frame_count ,video_stream->codec->time_base ,
                     audio_frame_count , audio_stream->codec->time_base ) <= 0){
        if(nativeYuv != NULL){
            LOGE("write video ");
            encodeYuv(nativeYuv);
        }
    }
    else {
        if(nativePcm != NULL){
            LOGE("write audio ");
            encodePcm(nativePcm);
        }
    }
}



int encodeCamera_muxer(jbyte *nativeYuv){
    int ret = 0 ;
//    LOGE("encodeCamera_muxer");
    video_frame_count ++;
    encode(nativeYuv , NULL);
    return ret;
}

int encodeAudio_muxer(jbyte *nativePcm){
    int ret = 0 ;
    encode(NULL , nativePcm);
    audio_frame_count++;
//    LOGE("encodeAudio_muxer");
    return ret;
}

int close_muxer(){
    LOGE("CLOSE_CAMERA_MUXER");
    return 1;
}