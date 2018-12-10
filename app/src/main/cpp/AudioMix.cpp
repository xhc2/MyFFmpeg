//
// Created by Administrator on 2018/12/7/007.
//

#include <my_log.h>

#include "AudioMix.h"
#include <stdint.h>
#include<stdio.h>
#include <unistd.h>
extern "C"{

#include <inttypes.h>
}


void custom_log5(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeg_log_filter.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};
AudioMix::AudioMix() {
    const  char *filter_descr = "[in0][in1]amix=inputs=2[out]";

    int ret;

    frame = av_frame_alloc();
    filt_frame = av_frame_alloc();
    int got_frame;

    if (!frame || !filt_frame) {
        perror("Could not allocate frame");
        exit(1);
    }
    av_register_all();
    avfilter_register_all();
    av_log_set_callback(custom_log5);
    const char* inputPath1 = "sdcard/FFmpeg/v1080_8000_flt_1.mp4";
    const char* inputPath2 = "sdcard/FFmpeg/test_8000_1_flt.mp4";
    if ((ret = open_input_file_1(inputPath1)) < 0)
    {
        LOGE( "open input file fail, ret: %d\n");
        return ;
    }
    if ((ret = open_input_file_2(inputPath2)) < 0)
    {
        LOGE( "open input file fail, ret: %d\n");
        return ;
    }
    if ((ret = init_filters(filter_descr)) < 0)
    {
        LOGE( "init filters fail, ret: \n");
        return ;
    }
    LOGE( "AudioMix success \n");
}

int AudioMix::open_input_file_1(const char *filename)
{
    int ret;
    AVCodec *dec;
    fmt_ctx1 = NULL;
    if ((ret = avformat_open_input(&fmt_ctx1, filename, NULL, NULL)) < 0) {
        LOGE( "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx1, NULL)) < 0) {
        LOGE( "Cannot find stream information\n");
        return ret;
    }

    /* select the audio stream */
    ret = av_find_best_stream(fmt_ctx1, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGE( "Cannot find an audio stream in the input file\n");
        return ret;
    }
    audio_stream_index_1 = ret;

//    AVCodecID codecID = fmt_ctx1->streams[audio_stream_index_1]->codecpar->codec_id;

//    if(codecID == AV_CODEC_ID_NONE){
//        LOGE( " open_input_file_2 AV_CODEC_ID_NONE  ");
//        return -1;
//    }
//    LOGE(" CODE NAME %d " ,codecID);
//    AVCodec *code = avcodec_find_decoder(codecID);
//
//    if(code == NULL){
//        LOGE( " avcodec_find_decoder faild !  ");
//        return -1;
//    }
        LOGE(" CODE NAME %s " , dec->name );
    dec_ctx1 = avcodec_alloc_context3(dec);
    if(dec_ctx1 == NULL){
        LOGE( " avcodec_alloc_context3 faild !  ");
        return -1;
    }

    avcodec_parameters_to_context(dec_ctx1, fmt_ctx1->streams[audio_stream_index_1]->codecpar);
    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx1, dec, NULL)) < 0) {
        LOGE( "Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

int AudioMix::open_input_file_2(const char *filename)
{
    int ret;
    AVCodec *dec;
    fmt_ctx2 = NULL;

    if ((ret = avformat_open_input(&fmt_ctx2, filename, NULL, NULL)) < 0) {
        LOGE( "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx2, NULL)) < 0) {
        LOGE( "Cannot find stream information\n");
        return ret;
    }

    /* select the audio stream */
    ret = av_find_best_stream(fmt_ctx2, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGE( "Cannot find an audio stream in the input file\n");
        return ret;
    }
    audio_stream_index_2 = ret;

//    AVCodecID codecID = fmt_ctx2->streams[audio_stream_index_2]->codecpar->codec_id;
//
//    if(codecID == AV_CODEC_ID_NONE){
//        LOGE( " open_input_file_2 AV_CODEC_ID_NONE  ");
//        return -1;
//    }
//
//    AVCodec *code = avcodec_find_decoder(codecID);
//    if(code == NULL){
//        LOGE( " avcodec_find_decoder faild !  ");
//        return -1;
//    }
    LOGE(" CODE 2 NAME %s " , dec->name);
    dec_ctx2 = avcodec_alloc_context3(dec);
    if(dec_ctx2 == NULL){
        LOGE( " avcodec_alloc_context3 faild !  ");
        return -1;
    }
//    dec_ctx2 = fmt_ctx2->streams[audio_stream_index_2]->codec;
//    av_opt_set_int(dec_ctx2, "refcounted_frames", 1, 0);
    avcodec_parameters_to_context(dec_ctx2, fmt_ctx2->streams[audio_stream_index_2]->codecpar);
    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx2, dec, NULL)) < 0) {
        LOGE( "Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}

AVFrame *AudioMix::decodePacket(AVCodecContext *decode, AVPacket *packet) {
    int result = avcodec_send_packet(decode, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(decode, frame);
        if (result < 0) {
            LOGE(" avcodec_receive_frame  faild %s ", av_err2str(result));
            av_frame_free(&frame);
            return NULL;
        }
        return frame;
    }
    av_frame_free(&frame);
    return NULL;
}


int AudioMix::startMix() {
    AVPacket *packet = av_packet_alloc();
    FILE *mixF = fopen("sdcard/FFmpeg/mixpcm.pcm" , "wb+");
    int ret ;
    int frame1Count = 0;
    int frame2Count = 0;
    while(true){
        if ((ret = av_read_frame(fmt_ctx1, packet)) < 0){
            LOGE("  read first file end ");
            break;
        }


        if(packet->stream_index == audio_stream_index_1){
            AVFrame *frame = decodePacket(dec_ctx1 , packet);
            av_packet_unref(packet);
            if(frame != NULL){
                frame1Count ++;
                LOGE(" read file 1  %d "  , frame1Count);
                if ((ret = av_buffersrc_add_frame_flags(buffersrc_ctx1, frame, 0)) < 0) {
                    LOGE("Error while feeding the audio filtergraph %s " , av_err2str(ret));
                    break;
                }
            }
        }
    }

    while(true){
        if ((ret = av_read_frame(fmt_ctx2, packet)) < 0){
            LOGE("  read second file end ");
            break;
        }
        if(packet->stream_index == audio_stream_index_2){
            AVFrame *frame = decodePacket(dec_ctx2 , packet);
            av_packet_unref(packet);
            if(frame != NULL){
                frame2Count++;
                LOGE(" read file 2 %d " , frame2Count);
                if (av_buffersrc_add_frame_flags(buffersrc_ctx2, frame, 0) < 0) {
                    LOGE("Error while feeding the audio filtergraph\n");
                    break;
                }
            }
        }
    }
    while (true) {

        ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            LOGE(  "buffersink get frame fail, ret: %s \n", av_err2str(ret));
            continue;
        }
        if (ret < 0)
        {
            LOGE(  "buffersink get frame fail, ret: %s \n", av_err2str(ret));
            break;
        }
        LOGE(" GET FILTER FRAME !");
        int size = filt_frame->nb_samples * av_get_channel_layout_nb_channels(av_frame_get_channel_layout(filt_frame));
        fwrite(filt_frame->data[0] , 1 ,size , mixF );
        av_frame_unref(filt_frame);
    }
    fclose(mixF);
    LOGE(" END ");
    return 0;
}

int AudioMix::init_filters(const char *filters_descr)
{
    char args1[512];
    char args2[512];
    int ret = 0;
    AVFilter *abuffersrc1  = avfilter_get_by_name("abuffer");
    AVFilter *abuffersrc2  = avfilter_get_by_name("abuffer");
    AVFilter *abuffersink = avfilter_get_by_name("abuffersink");

    AVFilterInOut* output[2];
    output[0] = avfilter_inout_alloc();
    output[1] = avfilter_inout_alloc();
//    AVFilterInOut *outputs1 = avfilter_inout_alloc();
//    AVFilterInOut *outputs2 = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();

    const enum AVSampleFormat out_sample_fmts[] = { AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_NONE };
    const int64_t out_channel_layouts[] = { AV_CH_LAYOUT_MONO, -1 };
    const int out_sample_rates[] = { 8000, -1 };
    const AVFilterLink *outlink;


    AVRational time_base_1 = fmt_ctx1->streams[audio_stream_index_1]->time_base;
    AVRational time_base_2 = fmt_ctx2->streams[audio_stream_index_2]->time_base;


    filter_graph = avfilter_graph_alloc();
    if (!output[0] || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        LOGE(" avfilter_graph_alloc faild !");
        return -1;
    }
    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx1->channel_layout)
        dec_ctx1->channel_layout = av_get_default_channel_layout(dec_ctx1->channels);

    snprintf(args1, sizeof(args1),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x00000004",
             time_base_1.num, time_base_1.den, dec_ctx1->sample_rate,
             av_get_sample_fmt_name(dec_ctx1->sample_fmt)/*, dec_ctx1->channel_layout*/);

    ret = avfilter_graph_create_filter(&buffersrc_ctx1, abuffersrc1, "in1",
                                       args1, NULL, filter_graph);

    if (ret < 0) {
        LOGE("Cannot create audio buffer source1 %d " , av_err2str(ret));
        return -1;
    }

    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx2->channel_layout)
        dec_ctx2->channel_layout = av_get_default_channel_layout(dec_ctx2->channels);
    snprintf(args2, sizeof(args2),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%I64u",
             time_base_2.num, time_base_2.den, dec_ctx2->sample_rate,
             av_get_sample_fmt_name(dec_ctx2->sample_fmt), dec_ctx2->channel_layout);

//    snprintf(args2, sizeof(args2),
//              "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%I64x",
//              1, 1, 2,
//              44100, 0x000004);

    ret = avfilter_graph_create_filter(&buffersrc_ctx2, abuffersrc2, "in2",
                                       args2, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create audio buffer source2");
        return -1;
    }

    ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create audio buffer sink\n");
        return -1;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output sample format\n");
        return -1;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output channel layout\n");
        return -1;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output sample rate\n");
        return -1;
    }

    output[0]->name       = av_strdup("in0");
    output[0]->filter_ctx = buffersrc_ctx1;
    output[0]->pad_idx    = 0;
    output[0]->next       = output[1];

    output[1]->name       = av_strdup("in1");
    output[1]->filter_ctx = buffersrc_ctx2;
    output[1]->pad_idx    = 0;
    output[1]->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;


//    AVFilterInOut* filter_outputs[2];
//    filter_outputs[0] = output[0];
//    filter_outputs[1] = output[1];

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, output, NULL)) < 0)//filter_outputs
    {
        LOGE( "parse ptr fail, ret\n");
        return -1;
    }
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
    {
        LOGE( "config graph fail\n");
        return -1;
    }
    outlink = buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args1, sizeof(args1), -1, outlink->channel_layout);
    LOGE(" SUCCESS %s" , args1);

    avfilter_inout_free(&inputs);
    avfilter_inout_free(output);
    return 1;
}
AudioMix::~AudioMix() {

}
