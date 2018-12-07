//
// Created by Administrator on 2018/12/7/007.
//

#include <my_log.h>
#include "AudioMix.h"




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
    const char* inputPath1 = "sdcard/FFmpeg/video/test.mp4";
    const char* inputPath2 = "sdcard/FFmpeg/video/v1080.mp4";
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
        LOGE( "init filters fail, ret: %d\n");
        return ;
    }
    LOGE( "AudioMix success \n");
}

int AudioMix::open_input_file_1(const char *filename)
{
    int ret;
    AVCodec *dec;

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
    dec_ctx1 = fmt_ctx1->streams[audio_stream_index_1]->codec;
    av_opt_set_int(dec_ctx1, "refcounted_frames", 1, 0);

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
    dec_ctx2 = fmt_ctx2->streams[audio_stream_index_2]->codec;
    av_opt_set_int(dec_ctx2, "refcounted_frames", 1, 0);

    /* init the audio decoder */
    if ((ret = avcodec_open2(dec_ctx2, dec, NULL)) < 0) {
        LOGE( "Cannot open audio decoder\n");
        return ret;
    }

    return 0;
}
int AudioMix::startMix() {
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

    AVFilterInOut *outputs1 = avfilter_inout_alloc();
    AVFilterInOut *outputs2 = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();

    static const enum AVSampleFormat out_sample_fmts[] = { AV_SAMPLE_FMT_S16,AV_SAMPLE_FMT_NONE};
    static const int64_t out_channel_layouts[] = { AV_CH_LAYOUT_MONO, -1 };
    static const int out_sample_rates[] = { 8000, -1 };
    const AVFilterLink *outlink;

    AVRational time_base_1 = fmt_ctx1->streams[audio_stream_index_1]->time_base;
    AVRational time_base_2 = fmt_ctx2->streams[audio_stream_index_2]->time_base;

    filter_graph = avfilter_graph_alloc();
    if (!outputs1 || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx1->channel_layout)
        dec_ctx1->channel_layout = av_get_default_channel_layout(dec_ctx1->channels);
    snprintf(args1, sizeof(args1),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
             time_base_1.num, time_base_1.den, dec_ctx1->sample_rate,
             av_get_sample_fmt_name(dec_ctx1->sample_fmt), dec_ctx1->channel_layout);
    ret = avfilter_graph_create_filter(&buffersrc_ctx1, abuffersrc1, "in1",
                                       args1, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create audio buffer source\n");
        goto end;
    }

    /* buffer audio source: the decoded frames from the decoder will be inserted here. */
    if (!dec_ctx2->channel_layout)
        dec_ctx2->channel_layout = av_get_default_channel_layout(dec_ctx2->channels);
    snprintf(args2, sizeof(args2),
            "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
             time_base_2.num, time_base_2.den, dec_ctx2->sample_rate,
             av_get_sample_fmt_name(dec_ctx2->sample_fmt), dec_ctx2->channel_layout);
    ret = avfilter_graph_create_filter(&buffersrc_ctx2, abuffersrc1, "in2",
                                       args2, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create audio buffer source\n");
        goto end;
    }
    /* buffer audio sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, abuffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create audio buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output sample format\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output channel layout\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output sample rate\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs1->name       = av_strdup("in0");
    outputs1->filter_ctx = buffersrc_ctx1;
    outputs1->pad_idx    = 0;
    outputs1->next       = outputs2;

    outputs2->name       = av_strdup("in1");
    outputs2->filter_ctx = buffersrc_ctx2;
    outputs2->pad_idx    = 0;
    outputs2->next       = NULL;
    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;


    AVFilterInOut* filter_outputs[2];
    filter_outputs[0] = outputs1;
    filter_outputs[1] = outputs2;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs1, NULL)) < 0)//filter_outputs
    {
        LOGE( "parse ptr fail, ret: %d\n");
        goto end;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
    {
        LOGE( "config graph fail, ret: %d\n");
        goto end;
    }

    /* Print summary of the sink buffer
     * Note: args buffer is reused to store channel layout string */
    outlink = buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args1, sizeof(args1), -1, outlink->channel_layout);
//    LOGE(NULL, AV_LOG_INFO, "Output: srate:%dHz fmt:%s chlayout:%s\n",
//           (int)outlink->sample_rate,
//           (char *)av_x_if_null(av_get_sample_fmt_name(outlink->format), "?"),
//           args1);

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs1);

    return ret;
}
AudioMix::~AudioMix() {

}
