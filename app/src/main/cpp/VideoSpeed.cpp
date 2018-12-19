//
// Created by Administrator on 2018/12/19/019.
//
#include <my_log.h>
#include "VideoSpeed.h"
//http://bbs.chinaffmpeg.com/forum.php?mod=viewthread&tid=598

VideoSpeed::VideoSpeed() {
    afc_input = NULL;
    afc_output = NULL;
}

int VideoSpeed::init(const char *inputPath, float speed, const char *outputPath) {
    int result = buildInput(inputPath);
    if(result < 0){
        LOGE(" buildInput FAILD ! ");
        return -1;
    }
    result = buildOutput(outputPath);
    if(result < 0){
        LOGE(" buildInput FAILD ! ");
        return -1;
    }
    result = buildVideoFilter(speed);
    if(result < 0){
        LOGE(" buildInput FAILD ! ");
        return -1;
    }
    return 1;
}

int VideoSpeed::buildInput(const char *input) {

    int ret = open_input_file(input , &afc_input);
    if(ret < 0){
        LOGE(" open_input_file faild !");
        return -1;
    }
    ret = getAudioDecodeContext(afc_input , &aCtxD);
    if(ret < 0 ){
        LOGE(" getAudioDecodeContext faild ! ");
        return -1;
    }
    audioStreamIndex = ret ;
    inputAudioStream = afc_input->streams[audioStreamIndex];

    ret = getVideoDecodeContext(afc_input , &vCtxD);
    if(ret < 0 ){
        LOGE(" getVideoDecodeContext faild ! ");
        return -1;
    }
    videoStreamIndex = ret ;
    inputVideoStream = afc_input->streams[videoStreamIndex];
    LOGE(" BUILD INPUT SUCCESS !");
    return 1;
}

int VideoSpeed::buildOutput(const char *output) {

    int ret = initOutput(output , &afc_output);
    if(ret < 0){
        LOGE(" initOutput FAILD ! ");
        return -1;
    }
    ret = addOutputVideoStream(afc_output , &vCtxE , *inputVideoStream->codecpar);
    if(ret < 0){
        LOGE(" addOutputVideoStream FAILD ! ");
        return -1;
    }
    videoOutputStreamIndex = ret ;
    outputVideoStream = afc_output->streams[videoOutputStreamIndex];

    ret = addOutputAudioStream(afc_output , &aCtxE , *inputAudioStream->codecpar);
    if(ret < 0){
        LOGE(" addOutputAudioStream FAILD ! ");
        return -1;
    }
    audioOutputStreamIndex = ret ;
    outputAudioStream = afc_output->streams[audioOutputStreamIndex];
    ret = writeOutoutHeader(afc_output , output);
    if(ret < 0){
        LOGE(" writeOutoutHeader FAILD ! ");
        return -1;
    }
    LOGE(" BUILD OUTPUT SUCCESS !");
    return 1;
}

int VideoSpeed::buildVideoFilter(float speed) {

    int ret = init_filters("setpts=PTS/2" , afc_input , vCtxD);
    if(ret < 0){
        LOGE(" init_filters faild ! ");
        return -1;
    }
    LOGE(" init filter success !!!! ");
    return 1;
}


void VideoSpeed::run() {

}

int VideoSpeed::startSpeed() {


    AVFrame *frame;
    AVFrame *filt_frame = av_frame_alloc();
    int ret = 0;
//    start();
//    decodeFlag = true;
    while (!isExit) {
        AVPacket *packet = av_packet_alloc();
        if ((ret = av_read_frame(afc_input, packet)) < 0) {
            av_packet_free(&packet);
//            readEnd = true;
            LOGE(" READ FRAME FAILD !");
            break;
        }
        if (packet->stream_index == audioStreamIndex) {
//            av_packet_rescale_ts(packet, fmtCtx->streams[audioStreamIndex]->time_base,
//                                 afcOutput->streams[getAudioOutputStreamIndex()]->time_base);
//            audioQue.push(packet);
            av_packet_free(&packet);
        } else if (packet->stream_index == videoStreamIndex) {
            frame = decodePacket(vCtxD, packet);
            av_packet_free(&packet);
            if (frame != NULL) {
                LOGE(" frame pts %lld " , frame->pts);
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
                    LOGE(" filt_frame pts %lld " , filt_frame->pts);
                    AVPacket *filtPkt = encodeFrame(filt_frame, vCtxE);
                    if (filtPkt != NULL) {
                        av_packet_rescale_ts(filtPkt,
                                             afc_input->streams[videoStreamIndex]->time_base ,
                                             afc_output->streams[videoOutputStreamIndex]->time_base);
                        LOGE(" WIRTE VIDEO FRAME !!! ");
                        av_write_frame(afc_output , filtPkt);
//                        videoQue.push(filtPkt);
                    }
                    av_frame_unref(filt_frame);
                }
            }
        }
    }
    av_frame_free(&filt_frame);
//    decodeFlag = false;
    LOGE(" END ");
    writeTrail(afc_output );
    return 1;
}


VideoSpeed::~VideoSpeed() {

}


