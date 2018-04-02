// Created by Administrator on 2018/1/20/020.
// http://blog.csdn.net/XIAIBIANCHENG/article/details/72810495 格式转换
// http://blog.csdn.net/dancing_night/article/details/45972361 音频的pts的计算

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
#include <libswresample/swresample.h>



int audio_size ;
int  frame_audio_index;
int record_audio_out_index;
//FILE *SRCFILE , *CONVERTFILE;
int frame_count = 0 ;

AVOutputFormat *ofmt_audio = NULL;
AVFormatContext *ofmt_ctx_audio ;
AVFrame  *audioFrame;
AVStream *audio_stream;
AVPacket *pkt_audio;
SwrContext *swr;
uint8_t *outs[2];


int init_audio(const char*output_path , int a_size){
    int ret = 0 ;
    audio_size = a_size;
    av_register_all();
//    SRCFILE = fopen("sdcard/FFmpeg/src_audio_16.pcm" , "wb+");
//    CONVERTFILE = fopen("sdcard/FFmpeg/src_audio_fltp.pcm" , "wb+");
    av_log_set_callback(custom_log);
    ret = avformat_alloc_output_context2(&ofmt_ctx_audio , NULL , NULL , output_path);
    if(ret < 0){
        LOGE(" OPEN AVFormatContext FAILD ");
        return -1 ;
    }
    ofmt_audio = ofmt_ctx_audio->oformat;
    initAudio_record();
    init_Sws();
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

//初始化格式转换器
int init_Sws(){
    swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout",  AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_MONO,  0);
    av_opt_set_int(swr, "in_sample_rate",     16000, 0);
    av_opt_set_int(swr, "out_sample_rate",    16000, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt",  AV_SAMPLE_FMT_S16, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP,  0);
    swr_init(swr);

    outs[0]=(uint8_t *)malloc(audio_size);
    outs[1]=(uint8_t *)malloc(audio_size);
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
    record_audio_out_index = audio_stream->index;

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
//    avcodec_fill_audio_frame(audioFrame ,audio_stream->codec->channels ,audio_stream->codec->sample_fmt ,outs_16,
//                             audio_size,  0 );

    /* the codec gives us the frame size, in samples,
     * we calculate the size of the samples buffer in bytes */
//    int buffer_size = av_samples_get_buffer_size(NULL, audio_stream->codec->channels, audio_stream->codec->frame_size ,
//                                                 audio_stream->codec->sample_fmt, 0);


    pkt_audio = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt_audio, audio_size);
    LOGE(" AUDIO SAMPLE %d " , audioFrame->nb_samples);
    return ret ;
}


int close_audio(){
    LOGE("CLOSE AUDIO ");
    av_write_trailer(ofmt_ctx_audio);
    if (audio_stream) {
        avcodec_close(audio_stream->codec);
        av_free(audioFrame);
    }
    avio_close(ofmt_ctx_audio->pb);

    if(!pkt_audio){
        av_free_packet(pkt_audio);
    }
    LOGE(" ofmt_ctx_audio %d ", ofmt_ctx_audio);
    if (!ofmt_ctx_audio) {
        avformat_free_context(ofmt_ctx_audio);
    }



//    AVOutputFormat *ofmt_audio = NULL;
//    AVFormatContext *ofmt_ctx_audio ;
//    AVFrame  *audioFrame;
//    AVStream *audio_stream;
//    AVPacket *pkt_audio;
//    SwrContext *swr;
//    uint8_t *outs[2];

    return 1;
}

int encode_audio_(jbyte *nativepcm){
    int ret = 1 ;
    LOGE("ENCODE_AUDIO_");
    swr_convert(swr , &outs , audio_size * 2 , &nativepcm ,audio_size / 2);
    audioFrame->data[0] = outs[0];
    audioFrame->data[1] = outs[1];
    /**
     * 一秒钟有num_pkt packet , 一个packet不止一帧
     * num_pkt = 采样率 / nb_samples;
     * 第n个包的pts = n * (（1 / timbase）/ num_pkt);
     * 很多音频时间基和采样率成倒数
     * pts = n * nb_samples;
     */
    audioFrame->pts = frame_count * audioFrame->nb_samples;
    LOGE(" AUDIO PTS %lld" , audioFrame->pts );

    int got_audio;
    ret = avcodec_encode_audio2(audio_stream->codec, pkt_audio, audioFrame, &got_audio);
    if(ret < 0){
        LOGE(" ENCODE AUDIO  FAILD ");
        return ret;
    }
    if(got_audio == 1){
        pkt_audio->stream_index = audio_stream->index;
        ret = av_write_frame(ofmt_ctx_audio , pkt_audio);
        if(ret < 0){
            return ret;
        }
        av_free_packet(pkt_audio);
        frame_count ++ ;
    }
    return ret;
}
