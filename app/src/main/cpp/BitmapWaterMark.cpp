//
// Created by Administrator on 2018/12/1/001.
//

#include <my_log.h>
#include "BitmapWaterMark.h"

BitmapWaterMark::BitmapWaterMark(const char *videoPath,  const char *outputPath, const char *logoPath, int x, int y) {
    fmtCtx = NULL;
    int result = open_input_file(videoPath, &fmtCtx);
    if (result < 0 || fmtCtx == NULL) {
        LOGE(" open_input_file faild ! ");
        return;
    }
    result = getVideoDecodeContext(fmtCtx, &decCtx);
    if (result < 0 || decCtx == NULL) {
        LOGE(" getVideoDecodeContext faild ! ");
        return;
    }
    videoStreamIndex = result;
    audioStreamIndex = getAudioStreamIndex(fmtCtx);
    if(audioStreamIndex == -1){
        LOGE(" find audio Stream faild ! ");
        return ;
    }
    LOGE(" INPUT VIDEO STREAM INDEX %d , AUDIO STREAM INDEX %d " , videoStreamIndex , audioStreamIndex);
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
    vpts = 0 ;
    apts = 0;
    readEnd = false;
    LOGE(" BitmapWaterMark SUCCESS !");
}

int BitmapWaterMark::buildOutput(const char *outputPath){
    int result = initOutput(outputPath , &afcOutput);
    if(result < 0){
        LOGE(" initOutput faild !");
        return -1;
    }
    result = addOutputVideoStream(afcOutput , &vCtxE ,  *fmtCtx->streams[videoStreamIndex]->codecpar);
    if(result < 0 || vCtxE == NULL){
        LOGE("addOutputVideoStream FAILD !");
        return -1;
    }
    videoOutputStreamIndex = result;
    result = addOutputAudioStream(afcOutput , NULL , *fmtCtx->streams[audioStreamIndex]->codecpar );
    if(result < 0 ){
        LOGE("addOutputAudioStream FAILD !");
        return -1;
    }
    audioOutputStreamIndex = result;
    result = writeOutoutHeader(afcOutput , outputPath);
    if(result < 0   ){
        LOGE(" writeOutoutHeader FAILD !");
        return -1;
    }
    return 1;
}


void BitmapWaterMark::startWaterMark() {

    AVFrame *frame;
    AVFrame *filt_frame = av_frame_alloc();
    int ret = 0;
    start();
    while (true) {
        AVPacket *packet = av_packet_alloc();
        if ((ret = av_read_frame(fmtCtx, packet)) < 0) {
            av_packet_free(&packet);
            readEnd = true;
            LOGE(" READ FRAME FAILD !");
            break;
        }
        if(packet->stream_index == audioStreamIndex){
            av_packet_rescale_ts(packet, fmtCtx->streams[audioStreamIndex]->time_base, afcOutput->streams[getAudioOutputStreamIndex()]->time_base);
            audioQue.push(packet);
        }
        else if (packet->stream_index == videoStreamIndex) {
            frame = decodePacket(decCtx, packet);
            av_packet_free(&packet);
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
                        break;
                    }

                    if (ret < 0) {
                        LOGE("GET FRAME FAILD !");
                        break;
                    }
                    filt_frame->pts = filt_frame->best_effort_timestamp;
                    AVPacket *filtPkt = encodeFrame(filt_frame , vCtxE);
                    if(filtPkt != NULL){
                        av_packet_rescale_ts(filtPkt, fmtCtx->streams[videoStreamIndex]->time_base/*buffersink_ctx->inputs[0]->time_base*/, afcOutput->streams[getVideoOutputStreamIndex()]->time_base);
                        videoQue.push(filtPkt);
                    }
                    av_frame_unref(filt_frame);
                }
            }
        }
    }
    av_frame_free(&filt_frame);
    LOGE(" END ");
}

void BitmapWaterMark::run(){
    int result ;
    while(!isExit){
        if (this->pause ) {
            threadSleep(2);
            continue;
        }
        pthread_mutex_lock(&mutex_pthread);
        LOGE(" videoQue %d ， audioQue %d  ", videoQue.size(), audioQue.size());
        if (audioQue.size() <= 0 || videoQue.size() <= 0) {
            pthread_mutex_unlock(&mutex_pthread);
            if(readEnd){
                break;
            }
            continue;
        }

        AVPacket *aPkt = audioQue.front();
        AVPacket *vPkt = videoQue.front();
        pthread_mutex_unlock(&mutex_pthread);

        if (av_compare_ts(apts, afcOutput->streams[audioOutputStreamIndex]->time_base, vpts,  afcOutput->streams[videoOutputStreamIndex]->time_base) < 0) {
            apts = aPkt->pts;
            result = av_interleaved_write_frame(afcOutput, aPkt);
            if (result < 0) {
                LOGE(" audio av_interleaved_write_frame faild ! %s ", av_err2str(result));
            }
            av_packet_free(&aPkt);
            audioQue.pop();
        } else {
            vpts = vPkt->pts;
            result = av_interleaved_write_frame(afcOutput, vPkt);

            if (result < 0) {
                LOGE(" video av_interleaved_write_frame faild ! %s", av_err2str(result));
            }
            av_packet_free(&vPkt);
            videoQue.pop();
        }

    }
    writeTrail(afcOutput);
}

void BitmapWaterMark::clearAllQue(){
    while(!audioQue.empty()){
        AVPacket *pkt = audioQue.front();
        if(pkt != NULL){
            av_packet_free(&pkt);
        }
        audioQue.pop();
    }
    while(!videoQue.empty()){
        AVPacket *pkt = videoQue.front();
        if(pkt != NULL){
            av_packet_free(&pkt);
        }
        videoQue.pop();
    }
}
BitmapWaterMark::~BitmapWaterMark() {
    this->stop();
    this->join();
    clearAllQue();
     
    if(fmtCtx != NULL){
        avformat_close_input(&fmtCtx);
        fmtCtx = NULL;
    }
    if(decCtx != NULL){
        avcodec_free_context(&decCtx);
        decCtx = NULL;
    }
    if(afcOutput != NULL){
        avformat_free_context(afcOutput);
        afcOutput = NULL;
    }
    if(vCtxE != NULL){
        avcodec_free_context(&vCtxE);
        vCtxE = NULL;
    }
}