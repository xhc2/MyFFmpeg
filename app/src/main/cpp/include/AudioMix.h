//
// Created by Administrator on 2018/12/7/007.
//

#ifndef MYFFMPEG_AUDIOMIX_H
#define MYFFMPEG_AUDIOMIX_H


#include "EditParent.h"

class AudioMix  {

private:
    int open_input_file_2(const char *filename) ;
    int open_input_file_1(const char *filename) ;
    int init_filters(const char *filters_descr) ;
    AVFrame *frame ;
    AVFrame *filt_frame ;
    AVFormatContext *fmt_ctx1;
    AVFormatContext *fmt_ctx2;

    AVCodecContext *dec_ctx1;
    AVCodecContext *dec_ctx2;

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx1;
    AVFilterContext *buffersrc_ctx2;

    AVFilterGraph *filter_graph;
    int audio_stream_index_1  ;
    int audio_stream_index_2 ;


public :
    AudioMix();
    int startMix();
    AVFrame *decodePacket(AVCodecContext *decode, AVPacket *packet);
    ~AudioMix();
};

#endif //MYFFMPEG_AUDIOMIX_H
