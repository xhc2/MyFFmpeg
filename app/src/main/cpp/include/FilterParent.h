//
// Created by Administrator on 2018/12/1/001.
//

#ifndef MYFFMPEG_FILTERPARENT_H
#define MYFFMPEG_FILTERPARENT_H

#include "EditParent.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
};

class FilterParent : public EditParent{
private :
    AVFilterGraph *filter_graph;
protected :
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
public :
    FilterParent();
    int init_filters(const char *filters_descr,AVFormatContext *fmt_ctx,AVCodecContext *dec_ctx);

    ~FilterParent();
};

#endif //MYFFMPEG_FILTERPARENT_H
