//
// Created by Administrator on 2018/12/1/001.
//

#ifndef MYFFMPEG_FILTERPARENT_H
#define MYFFMPEG_FILTERPARENT_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
};

class FilterParent {

protected :
    AVFormatContext *fmt_ctx;
    AVCodecContext *dec_ctx;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    int video_stream_index = -1;
    int64_t last_pts = AV_NOPTS_VALUE;
public :
    FilterParent();
    int open_input_file(const char *filename);
    int init_filters(const char *filters_descr);
    int64_t getVideoDuration();
    AVFrame* decodePacket(AVCodecContext *dec_ctx , AVPacket *packet);

    ~FilterParent();
};

#endif //MYFFMPEG_FILTERPARENT_H
