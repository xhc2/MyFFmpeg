//
// Created by Administrator on 2018/12/18/018.
//

#include <my_log.h>
#include "VideoMusic.h"

VideoMusic::VideoMusic() {
    afc_input_video = NULL;
    afc_input_audio = NULL;
    afc_output = NULL;
    vCtxD = NULL;
    audioSampleCount = 0;
    isExit = false;
    dealEnd = false;
    vpts = 0;
    apts = 0;
}

int VideoMusic::init(const char *inputPath, const char *musicPath, const char *outputPath) {
    int result = buildVideoInput(inputPath);
    if (result < 0) {
        LOGE(" build video input faild !");
        return -1;
    }
    result = buildAudioInput(musicPath);
    if (result < 0) {
        LOGE(" build audio input faild !");
        return -1;
    }
    result = buildOutput(outputPath);
    if (result < 0) {
        LOGE(" build Output input faild !");
        return -1;
    }
    return 1;
}

int VideoMusic::buildVideoInput(const char *inputPath) {
    int result = open_input_file(inputPath, &afc_input_video);
    if (result < 0) {
        LOGE(" open_input_file faild !");
        return -1;
    }
    LOGE(" afc_input_video->nb_streams %d ", afc_input_video->nb_streams);
    result = getVideoDecodeContext(afc_input_video, &vCtxD);
    if (result < 0) {
        LOGE(" getVideoDecodeContext faild !");
        return -1;
    }
    videoStreamIndex = result;
    videoInputStream = afc_input_video->streams[videoStreamIndex];
    duration = afc_input_video->duration;

    return 1;
}

int VideoMusic::buildAudioInput(const char *inputPath) {

    int result = open_input_file(inputPath, &afc_input_audio);
    if (result < 0) {
        LOGE(" open_input_file faild !");
        return -1;
    }
    result = getAudioDecodeContext(afc_input_audio, &aCtxD);
    if (result < 0) {
        LOGE(" getAudioDecodeContext faild !");
        return -1;
    }
    audioStreamIndex = result;
    audioInputStream = afc_input_audio->streams[audioStreamIndex];
    return 1;
}

int VideoMusic::buildOutput(const char *output) {

    int result = initOutput(output, &afc_output);
    if (result < 0) {
        LOGE(" init output faild !");
        return -1;
    }

    result = addOutputAudioStream(afc_output, &aCtxE, *audioInputStream->codecpar);
    if (result < 0) {
        LOGE(" addOutputAudioStream faild !");
        return -1;
    }
    audioOutStream = afc_output->streams[result];
    aDuration = AV_TIME_BASE / audioOutStream->codecpar->sample_rate;

    result = addOutputVideoStream(afc_output, &vCtxE, *videoInputStream->codecpar);
    if (result < 0) {
        LOGE(" addOutputVideoStream faild !");
        return -1;
    }
    videoOutStream = afc_output->streams[result];
    result = writeOutoutHeader(afc_output, output);
    if (result < 0) {
        LOGE(" writeOutoutHeader faild !");
        return -1;
    }
    LOGE(" buildOutput success !");
    return 1;
}


/**
 * 音频和视频需要写入的时候解码再编码。节约内存为主
 * 1.如果音频时间小于视频，那么音频循环
 * 2.如果音频时间大于视频，那么视频结束了就结束
 * @return
 */
int VideoMusic::startVideoMusic() {
    int result;
    AVPacket *pkt = av_packet_alloc();

    while (!isExit) {
        if (av_compare_ts(apts, audioOutStream->time_base, vpts, videoOutStream->time_base) < 0) {
            //audio
            result = av_read_frame(afc_input_audio, pkt);
            if (result < 0) {
                av_seek_frame(afc_input_audio , audioInputStream->index , 0 , AVSEEK_FLAG_ANY);
                LOGE(" SEEK AUDIO FRAME ");
                av_packet_unref(pkt);
                continue;
            }
            if(pkt->stream_index != audioInputStream->index){
                av_packet_unref(pkt);
                continue;
            }
            AVFrame *frame = decodePacket(aCtxD, pkt);
            av_packet_unref(pkt);
            if (frame != NULL) {
                audioSampleCount += frame->nb_samples;
                frame->pts = audioSampleCount * aDuration;
                AVPacket *apkt = encodeFrame(frame, aCtxE);
                av_frame_free(&frame);
                if (apkt != NULL) {
                    av_packet_rescale_ts(apkt , timeBaseFFmpeg , audioOutStream->time_base);
                    apts = apkt->pts;
                    apkt->stream_index = audioInputStream->index;
                    result = av_interleaved_write_frame(afc_output, apkt);
                    av_packet_free(&apkt);
                    if (result < 0) {
                        LOGE(" av_interleaved_write_frame FAILD ! ");
                    }
                }
            }
        } else {
            //video
            result = av_read_frame(afc_input_video, pkt);
            if (result < 0) {
                LOGE(" READ VIDEO END !");
                av_packet_unref(pkt);
                break;
            }
            if(pkt->stream_index != videoInputStream->index){
                av_packet_unref(pkt);
                continue;
            }
            AVFrame *frame = decodePacket(vCtxD, pkt);
            av_packet_unref(pkt);
            if (frame != NULL) {
                AVPacket *vpkt = encodeFrame(frame, vCtxE);
                av_frame_free(&frame);
                if (vpkt != NULL) {
                    progress = (int)(av_rescale_q(vpkt->pts , videoInputStream->time_base , timeBaseFFmpeg) / (float)duration * 100) ;
                    LOGE(" PROGRESS %d " , progress);
                    if(progress > 0){
                        progress --;
                    }
                    av_packet_rescale_ts(vpkt , videoInputStream->time_base , videoOutStream->time_base);
                    vpts = vpkt->pts;
                    vpkt->stream_index = videoOutStream->index;
                    result = av_interleaved_write_frame(afc_output, vpkt);
                    av_packet_free(&vpkt);
                    if (result < 0) {
                        LOGE(" av_interleaved_write_frame FAILD ! ");
                    }
                }
            }
        }
    }
    av_packet_free(&pkt);
    LOGE(" -------------------------------------- all end -------------------------------------- " );
    writeTrail(afc_output);
    dealEnd = true;
    progress = 100;

    return 1;
}

void VideoMusic::destroyInput(){
    if(aCtxD != NULL){
        avcodec_free_context(&aCtxD);
    }
    if(vCtxD != NULL){
        avcodec_free_context(&vCtxD);
    }
    if(afc_input_video != NULL){
        avformat_close_input(&afc_input_audio);
    }
    if(afc_input_audio != NULL){
        avformat_close_input(&afc_input_audio);
    }

}
void VideoMusic::destroyOutput(){
    if(aCtxE != NULL){
        avcodec_free_context(&aCtxE);
    }
    if(vCtxE != NULL){
        avcodec_free_context(&vCtxE);
    }
    if(afc_output != NULL){
        avformat_free_context(afc_output);
    }
}

VideoMusic::~VideoMusic() {
    isExit = true;
    while(!dealEnd){
        LOGE(" WAIT DEAL END ...");
    }
    destroyInput();
    destroyOutput();
    LOGE(" DESTROY SUCCESS !!!! ");
}



