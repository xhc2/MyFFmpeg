//
// Created by Administrator on 2018/12/1/001.
//

#include <my_log.h>
#include "FilterParent.h"


FilterParent::FilterParent() {
    avfilter_register_all();
}


int  FilterParent::init_filters(const char *filters_descr ,AVFormatContext *fmt_ctx , AVCodecContext *dec_ctx) {
    char args[512];
    int ret = 0;
    AVFilter *buffersrc = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    int videoStreamIndex = getVideoStreamIndex(fmt_ctx);
    if(videoStreamIndex < 0){
        LOGE(" getVideoStreamIndex FAILD ! ");
        return -1;
    }
    AVRational time_base = fmt_ctx->streams[videoStreamIndex]->time_base;
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_alloc faild !");
        return -1;
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             time_base.num, time_base.den,
             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_alloc faild !");
        return -1;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_alloc faild !");
        return -1;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_alloc faild !");
        return -1;
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
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;
    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_parse_ptr faild !");
        return -1;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        LOGE("avfilter_graph_config faild !");
        return -1;
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return 1;
}

FilterParent::~FilterParent() {
    avfilter_graph_free(&filter_graph);

}