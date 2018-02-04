//
// Created by Administrator on 2018/2/4/004.
//
#include "my_new_camera_muxer_filter.h"
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
#include <my_utils.h>

AVOutputFormat *ofmt = NULL;
const char *mp4_output_path;
int width, height, audio_size;
AVFormatContext *ofmt_ctx;
AVCodec *avVideoCode;
AVStream *video_stream;
AVCodec *avAudioCode;
AVStream *audio_stream;
int video_gop_size = 12;
SwrContext *swr;
uint8_t *outs[2];
int y_size;
AVFrame *video_frame;
AVFrame *audio_frame;
int videoFrameCount = 0;
int audioFrameCount = 0;
AVPacket videoPacket;
AVPacket audioPacket;
int64_t video_last_pts, audio_last_pts;
AVBitStreamFilterContext* h264bsfc;

int init_camera_filter(const char* outputPath  , int w , int h , int aSize){
    LOGE("init_camera_filter");
    int ret = 0;
    mp4_output_path = outputPath;
    width = w;
    height = h;
    y_size = w * h;
//    srcYuv = fopen("sdcard/FFmpeg/my_new_camera.yuv" , "wb+");
    audio_size = aSize;
    LOGE("W %d , h %d ,y_size %d", w, h, y_size);
    av_register_all();
    av_log_set_callback(custom_log);
//    ofmt = av_guess_format(NULL , outputPath , NULL);
    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, outputPath);

    if (ret < 0) {
        LOGE("AVFormatContext ALLOC CONTEXT FAILD !");
        return -1;
    }
    ofmt = ofmt_ctx->oformat;
    if (ofmt == NULL) {
        LOGE("AVOutputFormat FAILD!");
        return -1;
    }
    if (ofmt->audio_codec != AV_CODEC_ID_NONE) {

        ret = initMuxerAudio_filter();
        init_muxer_Sws_filter();
        if (ret < 0) {
            return -1;
        }
    }

    if (ofmt->video_codec != AV_CODEC_ID_NONE) {
        ret = initMuxerVideo_filter();
        if (ret < 0) {
            return -1;
        }
    }

    if (ofmt->subtitle_codec != AV_CODEC_ID_NONE) {

    }

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&ofmt_ctx->pb, outputPath, AVIO_FLAG_WRITE) < 0) {
            LOGE("OPEN OUTPUT FILE FAILD !");
            return -1;
        }
    }
    h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    ret = avformat_write_header(ofmt_ctx, NULL);
    av_init_packet(&audioPacket);
    av_init_packet(&videoPacket);
    //打印信息到log文件中
    av_dump_format(ofmt_ctx, 0, outputPath, 1);
    if (ret < 0) {
        LOGE(" WRITE HEADER FAILD !");
        return -1;
    }
    LOGE(" INIT SUCCESS ！");

    return 1;
}



int initMuxerVideo_filter() {
    int ret = 0;
    avVideoCode = avcodec_find_encoder(ofmt->video_codec);
    LOGE(" VIDEO CODE NAME %s" , avVideoCode->name);
    if (avVideoCode == NULL) {
        LOGE("FIND VIDEO CODE FAILD!");
        return -1;
    }
    video_stream = avformat_new_stream(ofmt_ctx, avVideoCode);
    if (video_stream == NULL) {
        LOGE(" VIDEO STREAM ALLOC FAILD !");
        return -1;
    }
    AVCodecContext *vCodeContext = video_stream->codec;
    vCodeContext->codec_type = AVMEDIA_TYPE_VIDEO;
    vCodeContext->bit_rate = 600000;
    vCodeContext->width = width;
    vCodeContext->height = height;
    vCodeContext->time_base.den = 25;
    vCodeContext->time_base.num = 1;
    vCodeContext->gop_size = video_gop_size;
    vCodeContext->pix_fmt = AV_PIX_FMT_YUV420P;
    vCodeContext->qmin = 10;
    vCodeContext->qmax = 51;
    vCodeContext->qcompress = 0.6f;
    vCodeContext->max_b_frames = 2;
    video_stream->time_base.den = 90000;
    video_stream->time_base.num = 1;
    ret = avcodec_open2(video_stream->codec, avVideoCode, NULL);
    if (ret < 0) {
        LOGE(" VIDEO AVCODE OPEN FAILD !");
        return -1;
    }
    video_frame = av_frame_alloc();
    video_frame->format = video_stream->codec->pix_fmt;
    video_frame->width = video_stream->codec->width;
    video_frame->height = video_stream->codec->height;
//    AVBitStreamFilterContext* mpeg4bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
//    if(mpeg4bsfc == NULL){
//        LOGE("mpeg4bsfc FAILD !");
//        return -1;
//    }
//    uint8_t *data[4];
//    int linesize[4]; //这个不知道怎么使用。有问题
//    ret = av_image_fill_arrays(data , linesize , NULL , video_stream->codec->pix_fmt , width , height ,0);


    int pic_size = avpicture_get_size(video_stream->codec->pix_fmt, video_stream->codec->width,
                                      video_stream->codec->height);

    LOGE(" pic_size %d ", pic_size);

    uint8_t *picture_buf = (uint8_t *) av_malloc(pic_size);
    /**
     * Setup the data pointers and linesizes based on the specified image parameters and the provided array.
     */
    avpicture_fill((AVPicture *) video_frame, picture_buf, video_stream->codec->pix_fmt,
                   video_stream->codec->width, video_stream->codec->height);

    if (ret < 0) {
        LOGE(" AVIMAGE FILL ARRAY FAILD !");
        return -1;
    }

    // some formats want stream headers to be separate
    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        ofmt_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    LOGE(" INIT VIDEO SUCCESS !");
    return 1;
}


int initMuxerAudio_filter() {
    int ret = 0;
    avAudioCode = avcodec_find_encoder(ofmt->audio_codec);
    if (avAudioCode == NULL) {
        LOGE("FIND AUDIO CODE FAILD!");
        return -1;
    }
    audio_stream = avformat_new_stream(ofmt_ctx, avAudioCode);
    if (audio_stream == NULL) {
        LOGE(" AUDIO STREAM ALLOC FAILD !");
        return -1;
    }
    AVCodecContext *audio_context = audio_stream->codec;
    audio_context->codec_type = AVMEDIA_TYPE_AUDIO;
    audio_context->channel_layout = AV_CH_LAYOUT_MONO;
    audio_context->channels = av_get_channel_layout_nb_channels(audio_context->channel_layout);
    audio_context->bit_rate = 64000;
    audio_context->sample_rate = 44100;
    audio_context->sample_fmt = AV_SAMPLE_FMT_FLTP;//*avAudioCode->sample_fmts;
    ret = avcodec_open2(audio_context, avAudioCode, NULL);
    LOGE(" AUDIO CODE NAME %s" ,avAudioCode->name );
    if (ret < 0) {
        LOGE(" avcodec_open2 AUDIO FAILD ! ");
        return -1;
    }
    if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        ofmt_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;

    audio_frame = av_frame_alloc();
    audio_frame->format = audio_stream->codec->sample_fmt;
    audio_frame->nb_samples = audio_stream->codec->frame_size;
    audio_frame->channel_layout = audio_stream->codec->channel_layout;

    LOGE(" INIT AUDIO MUXER SUCCESS !");
    return 1;
}

int init_muxer_Sws_filter() {
    swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr, "in_sample_rate", 16000, 0);
    av_opt_set_int(swr, "out_sample_rate", 16000, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
    swr_init(swr);
    outs[0] = (uint8_t *) malloc(audio_size);
    outs[1] = (uint8_t *) malloc(audio_size);
}


int encode_filter(jbyte *nativeYuv , jbyte *nativePcm){
//    LOGE("codec %d");

    int writeVideo = av_compare_ts(video_last_pts ,video_stream->time_base ,
                                   audio_last_pts , audio_stream->codec->time_base );
    LOGE("cur_pts_v = %lld  ，cur_pts_a = %lld , writeVideo = %d " , video_last_pts  , audio_last_pts , writeVideo);
    if( writeVideo <= 0){
        if(nativeYuv != NULL){
            encodeYuv__filter(nativeYuv);
        }
        else{
            return -1;
        }
    }
    else {
        if(nativePcm != NULL){
            encodePcm__filter(nativePcm);
        }
        else{
            return -1;
        }
    }
    return 1;
}

int encodeYuv__filter(jbyte *nativeYuv){

    utils_nv21ToYv12(nativeYuv ,y_size);
    video_frame->data[0] = (uint8_t *) nativeYuv;
    video_frame->data[1] = (uint8_t *) nativeYuv + y_size;
    video_frame->data[2] = (uint8_t *) nativeYuv + y_size * 5 / 4;
//    int64_t temp = av_frame_get_best_effort_timestamp(video_frame);
//    LOGE(" TEMP %lld" , temp);
//    fwrite(nativeYuv , 1 ,y_size * 3 / 2 , srcYuv );
    int got_picture;
    if (avcodec_encode_video2(video_stream->codec, &videoPacket, video_frame, &got_picture) < 0) {
        LOGE("ENCODE VIDEO FAILD ！");
        return -1;
    }
    if (got_picture == 1) {
        videoPacket.duration = av_rescale_q(1, video_stream->codec->time_base,
                                            video_stream->time_base);
        videoPacket.pts = videoFrameCount * videoPacket.duration;
        videoPacket.dts = videoPacket.pts;
        videoPacket.stream_index = video_stream->index;
        video_last_pts = videoPacket.pts;
        interleaved_write_filter(&videoPacket , NULL);
        av_free_packet(&videoPacket);
        videoFrameCount++;
    }
    return 1;
}

int encodePcm__filter(jbyte *nativePcm){
    int ret ;
    int count = swr_convert(swr, &outs, audio_size * 2, &nativePcm, audio_size / 2);
    audio_frame->data[0] = outs[0];
    audio_frame->data[1] = outs[1];
    audio_frame->pts = audioFrameCount * audio_frame->nb_samples;
    int got_audio;
    ret = avcodec_encode_audio2(audio_stream->codec, &audioPacket, audio_frame, &got_audio);
    if (ret < 0) {
        LOGE(" ENCODE AUDIO FAILD !");
        return -1;
    }
    if (got_audio == 1) {
        audioPacket.stream_index = audio_stream->index;
        audio_last_pts = audio_frame->pts;
        interleaved_write_filter(  NULL ,  &audioPacket);
        av_free_packet(&audioPacket);
        audioFrameCount++;
    }
    return 1;
}

int encodeCamera_muxer_filter(jbyte *nativeYuv) {
    int ret = encode_filter(nativeYuv , NULL);
    return ret;
}


int encodeAudio_muxer_filter(jbyte *nativePcm) {
    int ret = 0;
    ret = encode_filter(NULL , nativePcm);
    return ret;
}

int close_muxer() {
    av_write_trailer(ofmt_ctx);
//    fclose(srcYuv);
    LOGE("CLOSE SUCCESS ");
    return -1;
}

int interleaved_write_filter(AVPacket *yuvPkt, AVPacket *pcmPkt) {
    if(yuvPkt != NULL){
        if (av_interleaved_write_frame(ofmt_ctx, yuvPkt) < 0) {
            LOGE(" WRITE VIDEO_FRAME FAILD ! ");
            return -1;
        }
    }

    if(pcmPkt != NULL){
        if (av_interleaved_write_frame(ofmt_ctx, pcmPkt) < 0) {
            LOGE(" WRITE AUDIO_FRAME FAILD ! ");
            return -1;
        }
    }

    if (yuvPkt != NULL) {
        av_free_packet(&videoPacket);
    }

    if (pcmPkt != NULL) {
        av_free_packet(pcmPkt);
    }

    return -1;
}
