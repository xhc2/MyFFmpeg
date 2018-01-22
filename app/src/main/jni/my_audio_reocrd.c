//
// Created by Administrator on 2018/1/20/020.
//
#include "my_audio_record.h"
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


int audio_size ;
AVOutputFormat *ofmt_audio = NULL;
AVFormatContext *ofmt_ctx_audio ;
int  frame_audio_index;
AVFrame  *audioFrame;
AVStream *audio_stream;
AVPacket *pkt_audio;
int audio_out_index;
int init_audio(const char*output_path , int a_size){
    int ret = 0 ;
    audio_size = a_size;
    av_register_all();
    av_log_set_callback(custom_log);
    ret = avformat_alloc_output_context2(&ofmt_ctx_audio , NULL , NULL , output_path);
    if(ret < 0){
        LOGE(" OPEN AVFormatContext FAILD ");
        return -1 ;
    }
    ofmt_audio = ofmt_ctx_audio->oformat;
    initAudio_record();
    /**
* AVFMT_NOFILE需要一个没有打开的File
*/
    if(!(ofmt_audio->flags & AVFMT_NOFILE)){
        if(avio_open(&ofmt_ctx_audio->pb , output_path , AVIO_FLAG_WRITE) < 0){
            LOGE("OPEN OUTPUT FILE FAILD !");
            return -1;
        }
    }
    if(avformat_write_header(ofmt_ctx_audio , NULL) < 0){
        LOGE("write_header faild ");
        return -1;
    }
    LOGE("init_camera_muxer SUCCESS %s"  , ofmt_audio->name);
    return ret ;
}


int initAudio_record(){
    int ret = -1 ;
    audio_stream  = avformat_new_stream(ofmt_ctx_audio , 0);
    if (audio_stream==NULL){
        LOGE(" audio_st FAILD ");
        return -1;
    }
    audio_stream->codec->codec_id = ofmt_ctx_audio->oformat->audio_codec;
    audio_stream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    //android 是输入s16格式，需要自己转换下
    audio_stream->codec->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audio_stream->codec->sample_rate= 44100;
    audio_stream->codec->channel_layout = AV_CH_LAYOUT_MONO;
    audio_stream->codec->channels = av_get_channel_layout_nb_channels(audio_stream->codec->channel_layout);
    audio_stream->codec->bit_rate = 64000;
    audio_out_index = audio_stream->index;

    AVCodec *avCodec = avcodec_find_encoder(audio_stream->codec->codec_id );
    if(avCodec == NULL){
        LOGE(" AUDIO CODE FAILD ");
        return -1;
    }
    if (avcodec_open2(audio_stream->codec, avCodec, NULL) < 0) {
        LOGE("Failed to open audio encoder! \n");
        return -1;
    }
    if(ofmt_ctx_audio->oformat->flags & AVFMT_GLOBALHEADER){
        audio_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    audioFrame = av_frame_alloc();
    audioFrame->format = audio_stream->codec->sample_fmt;
    audioFrame->nb_samples = audio_stream->codec->frame_size;
    audioFrame->channel_layout = audio_stream->codec->channel_layout;
    /* the codec gives us the frame size, in samples,
     * we calculate the size of the samples buffer in bytes */
    int buffer_size = av_samples_get_buffer_size(NULL, audio_stream->codec->channels, audio_stream->codec->frame_size ,
                                                 audio_stream->codec->sample_fmt, 0);


    pkt_audio = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt_audio, audio_size);
    LOGE(" AUDIO SAMPLE %d " , audioFrame->nb_samples);
    return ret ;
}


int close_audio(){
    LOGE("CLOSE AUDIO ");
    return 1;
}

int encode_audio_(jbyte *nativepcm){
    LOGE("ENCODE_AUDIO_");
    return 1;
}
