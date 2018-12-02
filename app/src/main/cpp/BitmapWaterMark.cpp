//
// Created by Administrator on 2018/12/1/001.
//

#include <my_log.h>
#include "BitmapWaterMark.h"

BitmapWaterMark::BitmapWaterMark(const char *videoPath,  const char *outputPath, const char *logoPath, int x, int y) {
    fmtCtx = NULL;
    int result = open_input_file(videoPath, &fmtCtx);
    if (result < 0) {
        LOGE(" open_input_file faild ! ");
        return;
    }
    result = getVideoDecodeContext(fmtCtx, &decCtx);
    if (result < 0) {
        LOGE(" getVideoDecodeContext faild ! ");
        return;
    }
    videoStreamIndex = result;
    result =  buildOutput(outputPath);
    if(result < 0){
        LOGE(" buildOutput faild !");
        return ;
    }
    char outChar[512];
    sprintf(outChar, filter_descr, logoPath, x, y);
    result = init_filters(outChar, fmtCtx, decCtx);
    if (result < 0) {
        LOGE(" init_filters FAILD !");
        return;
    }
    LOGE(" BitmapWaterMark SUCCESS !");
}

int BitmapWaterMark::buildOutput(const char *outputPath){
    int result = initOutput(outputPath , &afcOutput);
    if(result < 0){
        LOGE(" initOutput faild !");
        return -1;
    }
    result = addOutputVideoStream(afcOutput , &vCtxE , decCtx->width , decCtx->height);
    if(result < 0){
        LOGE("addOutputVideoStream FAILD !");
        return -1;
    }
    result = writeOutoutHeader(afcOutput , outputPath);
    if(result < 0   ){
        LOGE(" writeOutoutHeader FAILD !");
        return -1;
    }
    return 1;
}
void BitmapWaterMark::startWaterMark() {
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame;
    AVFrame *filt_frame = av_frame_alloc();
    int ret = 0;
    while (true) {
        if ((ret = av_read_frame(fmtCtx, packet)) < 0) {
            av_packet_unref(packet);
            LOGE(" READ FRAME FAILD !");
            break;
        }

        if (packet->stream_index == videoStreamIndex) {
            frame = decodePacket(decCtx, packet);
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
                    AVPacket *filtPkt = encodeFrame(filt_frame , vCtxE);
                    if(filtPkt != NULL){
                        av_write_frame( afcOutput, filtPkt);
                        av_packet_free(&filtPkt);
                    }
//                    fwrite(filt_frame->data[0], 1, ySize, fileOut);
//                    fwrite(filt_frame->data[1], 1, ySize / 4, fileOut);
//                    fwrite(filt_frame->data[2], 1, ySize / 4, fileOut);
                    av_frame_unref(filt_frame);
                }
            }
        }
        av_packet_unref(packet);
    }
    writeTrail(afcOutput);
    LOGE(" END ");
}


BitmapWaterMark::~BitmapWaterMark() {

}