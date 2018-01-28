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

int width ;
int height ;
int audio_size ;
AVOutputFormat *ofmt = NULL;
AVFormatContext *ofmt_ctx ;
int my_video_stream_index = -1 ;
int my_audio_stream_index = -1;
//保存码流的pts
int64_t cur_pts_v = 0 , cur_pts_a = 0;
int64_t video_duration  , audio_duration;
int frame_video_index , frame_audio_index;
AVBitStreamFilterContext * my_h264bsfc;
AVBitStreamFilterContext * my_aacbsfc ;

AVFrame *videoFrame , *audioFrame;
AVStream *video_stream;
AVStream *audio_stream;
int y_size = 0;
AVPacket *pkt_video , *pkt_audio;
SwrContext *swr;
uint8_t *outs[2];
//FILE *SRCFILE , *CONVERTFILE;

int init_camera_muxer(const char *outputPath , int w , int h , int aSize){
    int ret = 0 ;
    width = w;
    height = h ;
    y_size = width * height ;
    audio_size = aSize;
    av_register_all();
    av_log_set_callback(custom_log);
    ret = avformat_alloc_output_context2(&ofmt_ctx , NULL , NULL , outputPath);
    if(ret < 0){
        LOGE(" OPEN AVFormatContext FAILD ");
        return -1 ;
    }
    my_h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    my_aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
    ofmt = ofmt_ctx->oformat;
    initMuxerVideo();
    initMuxerAudio();
    init_muxer_Sws();
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
//    h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
//    aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
    LOGE("init_camera_muxer SUCCESS %s"  , ofmt->name);



    for(int i = 0 ;i < ofmt_ctx->nb_streams ; ++ i){
        if(ofmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            LOGE("video media index %d " , i);
        }
        else if(ofmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            LOGE("audio media index %d " , i);
        }
    }

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
    video_stream->codec->gop_size = 30;
    video_stream->codec->time_base.num = 1;
    video_stream->codec->time_base.den = 25;
    //最小视频量化标度，设定最小质量。
    video_stream->codec->qmin = 30;
    video_stream->codec->qmax = 51;

    video_duration = video_stream->time_base.den / 25;
    my_video_stream_index = video_stream->index;
    LOGE(" VIDEO_OUTINDEX %d " , my_video_stream_index);
    LOGE(" video_duration %lld " ,video_duration );

    AVCodec *avCodec = avcodec_find_encoder(video_stream->codec->codec_id );

    if (avcodec_open2(video_stream->codec, avCodec, NULL) < 0) {
        LOGE("Failed to open video encoder! \n");
        return -1;
    }
    LOGE(" vidoe codec name %s" , video_stream->codec->codec_name);
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

    pkt_video = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt_video, pic_size);
    return ret ;
}

//http://ffmpeg.org/doxygen/3.2/structAVFrame.html#details
//http://www.samirchen.com/ffmpeg-tutorial-5/ 参考pts博客
//http://ffmpeg.org/doxygen/3.2/transcode__aac_8c_source.html#l00582 关于audio pts的处理
//http://blog.csdn.net/XIAIBIANCHENG/article/details/72810495 s16 转 AV_SAMPLE_FMT_FLTP
int initMuxerAudio(){
    int ret = -1 ;
    audio_stream  = avformat_new_stream(ofmt_ctx , 0);
    if (audio_stream==NULL){
        LOGE(" audio_st FAILD ");
        return -1;
    }
    audio_stream->codec->codec_id = ofmt_ctx->oformat->audio_codec;
    audio_stream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    //android 是输入s16格式，需要自己转换下
    audio_stream->codec->sample_fmt = AV_SAMPLE_FMT_FLTP;
    audio_stream->codec->sample_rate= 44100;
    audio_stream->codec->channel_layout = AV_CH_LAYOUT_MONO;
    audio_stream->codec->channels = av_get_channel_layout_nb_channels(audio_stream->codec->channel_layout);
    audio_stream->codec->bit_rate = 64000;
    my_audio_stream_index = audio_stream->index;
    audio_duration = 1024 * audio_stream->time_base.den / audio_stream->codec->sample_rate;
    LOGE(" audio_stream_index %d " , my_audio_stream_index );

    AVCodec *avCodec = avcodec_find_encoder(audio_stream->codec->codec_id );
    if(avCodec == NULL){
        LOGE(" AUDIO CODE FAILD ");
        return -1;
    }
    if (avcodec_open2(audio_stream->codec, avCodec, NULL) < 0) {
        LOGE("Failed to open audio encoder! \n");
        return -1;
    }

    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
        audio_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    audioFrame = av_frame_alloc();
    audioFrame->format = audio_stream->codec->sample_fmt;
    audioFrame->nb_samples = audio_stream->codec->frame_size;
    audioFrame->channel_layout = audio_stream->codec->channel_layout;
    /* the codec gives us the frame size, in samples,
     * we calculate the size of the samples buffer in bytes */
//    int buffer_size = av_samples_get_buffer_size(NULL, audio_stream->codec->channels, audio_stream->codec->frame_size ,
//                                                 audio_stream->codec->sample_fmt, 0);
    pkt_audio = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt_audio, audio_size);
    LOGE(" AUDIO SAMPLE %d " , audioFrame->nb_samples);
    return ret ;
}
//初始化格式转换器
int init_muxer_Sws(){
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


int encodeYuv_(jbyte *nativeYuv){
    videoFrame->data[0] = (uint8_t *)nativeYuv;
    videoFrame->data[1] = (uint8_t *) nativeYuv + y_size;
    videoFrame->data[2] =(uint8_t *) nativeYuv + y_size * 5 / 4;
    videoFrame->pts = frame_video_index * (video_stream->time_base.den) / ((video_stream->time_base.num) * 25);
    cur_pts_v = videoFrame->pts;
    pkt_video->dts = cur_pts_v;
    int got_picture = 0;
    int ret = avcodec_encode_video2(video_stream->codec, pkt_video, videoFrame, &got_picture);
    if(ret < 0){
        LOGE(" ENCODE VIDEO FAILD");
        return -1;
    }
    if (got_picture == 1) {
        pkt_video->stream_index = my_video_stream_index;
        av_bitstream_filter_filter(my_h264bsfc, video_stream->codec, NULL, &pkt_video->data, &pkt_video->size,pkt_video->data, pkt_video->size, 0);
        interleaved_write(pkt_video , NULL);
        frame_video_index ++;
    }
    return 1;
}

int encodePcm_(jbyte *nativePcm){

    int count = swr_convert(swr,&outs , audio_size * 2 , &nativePcm ,audio_size / 2);
    audioFrame->data[0] =(uint8_t*)outs[0];
    audioFrame->data[1] =(uint8_t*)outs[1];
    audioFrame->pts = frame_audio_index * audioFrame->nb_samples;
    cur_pts_a = audioFrame->pts;
    pkt_audio->dts = cur_pts_a;
    int got_audio = -1;
    int ret = -1;
    ret = avcodec_encode_audio2(audio_stream->codec , pkt_audio , audioFrame , &got_audio);
    if(ret < 0){
        LOGE(" ENCODE AUDIO FAILD !");
        return -1;
    }
    if(got_audio == 1){
        pkt_audio->stream_index = my_audio_stream_index;
        av_bitstream_filter_filter(my_aacbsfc, audio_stream->codec, NULL, &pkt_audio->data, &pkt_audio->size,pkt_audio->data, pkt_audio->size, 0);
        interleaved_write(NULL , pkt_audio);
        frame_audio_index++;
    }

    return 1;
}

//http://ffmpeg.org/doxygen/3.2/tests_2movenc_8c_source.html#l00246 可以参考下
int interleaved_write(AVPacket *yuvPkt , AVPacket *pcmPkt){


        //写视频
        if(yuvPkt != NULL){
            LOGE(" WRITE YUV PKT ");

            if (av_interleaved_write_frame(ofmt_ctx, yuvPkt) < 0) {
                LOGE( "Error muxing yuv packet\n");
            }
            av_free_packet(yuvPkt);
        }
        else if(pcmPkt != NULL){
            LOGE(" WRITE PCM PKT ");

            if (av_interleaved_write_frame(ofmt_ctx, pcmPkt) < 0) {
                LOGE( "Error muxing PCM packet\n");
            }
            av_free_packet(pcmPkt);
        }

    if(pcmPkt != NULL){
        av_free_packet(pcmPkt);
    }

    if(yuvPkt != NULL){
        av_free_packet(yuvPkt);

    }
    return 1;
}

int tempIndex = 0;

int encode(jbyte *nativeYuv , jbyte *nativePcm){
    //这里永远都是等于1，不知道为什么。 http://blog.csdn.net/dancing_night/article/details/46472477

//    tempIndex ++;测试代码
//    int writeVideo = tempIndex % 2 == 0 ? 0 : 1;

    int writeVideo = av_compare_ts(cur_pts_v ,video_stream->codec->time_base ,
                                   cur_pts_a , audio_stream->codec->time_base );
//    writeVideo = 0;
    LOGE("cur_pts_v = %lld  ，cur_pts_a = %lld , writeVideo = %d " , cur_pts_v  , cur_pts_a , writeVideo);

    if( writeVideo <= 0){
        if(nativeYuv != NULL){
            encodeYuv_(nativeYuv);
        }
    }
    else {
        if(nativePcm != NULL){
            encodePcm_(nativePcm);
        }
    }
    return 1;
}

int encodeCamera_muxer(jbyte *nativeYuv){
    int ret = 0 ;
//    LOGE("encodeCamera_muxer");
    encode(nativeYuv , NULL);
    return ret;
}

int encodeAudio_muxer(jbyte *nativePcm){
    int ret = 0 ;
    encode(NULL , nativePcm);
//    LOGE("encodeAudio_muxer");
    return ret;
}

int close_muxer(){
    LOGE("CLOSE_CAMERA_MUXER");
    av_write_trailer(ofmt_ctx);
    return 1;
}