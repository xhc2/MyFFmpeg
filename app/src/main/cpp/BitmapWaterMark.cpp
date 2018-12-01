//
// Created by Administrator on 2018/12/1/001.
//

#include <my_log.h>
#include "BitmapWaterMark.h"

BitmapWaterMark::BitmapWaterMark(const char *videoPath, const char *logoPath, int x, int y) {
    int result = open_input_file(videoPath);
    if (result < 0) {
        LOGE(" open_input_file faild ! ");
        return;
    }
    const char *filter_descr = "movie=%s[wm];[in][wm]overlay=%d:%d[out]";
    char outChar[512];
    sprintf(outChar, filter_descr, logoPath, x, y);
    result = init_filters(outChar);
    if (result < 0) {
        LOGE(" init_filters FAILD !");
        return;
    }
    LOGE(" BitmapWaterMark SUCCESS !");
}

void BitmapWaterMark::startWaterMark() {
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame;
    AVFrame *filt_frame = av_frame_alloc();
    int ret = 0;
    FILE *fileOut = fopen("sdcard/FFmpeg/filter.yuv", "wb+");
    while (true) {
        if ((ret = av_read_frame(fmt_ctx, packet)) < 0) {
            av_packet_unref(packet);
            LOGE(" READ FRAME FAILD !");
            break;
        }

        if (packet->stream_index == video_stream_index) {
            frame = decodePacket(dec_ctx, packet);
            if (frame != NULL) {
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) <
                    0) {
                    LOGE(" av_buffersrc_add_frame_flags !");
                    av_frame_free(&frame);
                    break;
                }
                av_frame_free(&frame);
                while (true) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        LOGE("av_buffersink_get_frame FAILD ! %s ", av_err2str(ret));
                        break;
                    }

                    if (ret < 0) {
                        LOGE("GET FRAME FAILD !");
                        break;
                    }
                    int ySize = filt_frame->width * filt_frame->height;
//                    buffersink_ctx->inputs[0]->time_base
                    LOGE("WRITE FRAME ...linesize 0 %d , linesize 1 %d , linesize 2 %d ",
                         filt_frame->linesize[0], filt_frame->linesize[1], filt_frame->linesize[2]);
                    fwrite(filt_frame->data[0], 1, ySize, fileOut);
                    fwrite(filt_frame->data[1], 1, ySize / 4, fileOut);
                    fwrite(filt_frame->data[2], 1, ySize / 4, fileOut);
                    av_frame_unref(filt_frame);
                }
            }
        }
        av_packet_unref(packet);
    }
    LOGE(" END ");
}


BitmapWaterMark::~BitmapWaterMark() {

}