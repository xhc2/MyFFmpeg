//
// Created by Administrator on 2018/10/15/015.
// http://ffmpeg.org/doxygen/3.4/remuxing_8c-example.html#a25 可以参考
//

/**
 * 现在改到任意帧，也就是需要解码视频帧，不然会有花屏的情况。因为第一帧不是关键帧。
 * 音频帧就不用解码了。直接写入。
 *
 */
#include "VideoClip.h"

VideoClip::VideoClip(const char* path , const char* output ,  int startSecond , int endSecond){
    this->startSecond = startSecond ;
    this->endSecond = endSecond;

    int pathLen = strlen(path);
    pathLen ++;
    this->path = (char *)malloc(pathLen);
    strcpy(this->path  , path);

    int outputPathLen = strlen(output);
    outputPathLen++;
    this->outputPath = (char *)malloc(outputPathLen);
    strcpy(this->outputPath  , output);

    findKeyFrame = false;

}

int VideoClip::initInput(){
    int result = 0;
    afc_input = NULL;
    result = avformat_open_input(&afc_input, path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return -1;
    }
    result = avformat_find_stream_info(afc_input, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return -1;
    }

    for(int i = 0 ;i < afc_input->nb_streams ; ++ i){
        AVStream *stream = afc_input->streams[i];
        if(stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoStream = stream;
            video_index = i;
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        }
        else if(stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioStream = stream;
            audio_index = i;
        }
    }

    if(videoCodecD == NULL){
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    vCtxD = avcodec_alloc_context3(videoCodecD);

    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }

    avcodec_parameters_to_context(vCtxD, afc_input->streams[video_index]->codecpar);

    result = avcodec_open2(vCtxD, NULL, NULL);

    if (result != 0) {
        LOGE("ctx avcodec_open2 Faild !");
        return -1;
    }
    LOGE(" init input success ");

    return 1;
}

int VideoClip::initOutput(){
    int result = 0 ;
    afc_output = NULL;
    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, outputPath);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s " , av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    if(addVideoOutputStream() < 0){
        return -1;
    }
    if(addAudioOutputStream() < 0){
        return -1;
    }

    if (!(afot->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc_output->pb, outputPath, AVIO_FLAG_WRITE);
        if (result < 0) {
            LOGE(  "Could not open output file %s ", outputPath);
            return -1;
        }
    }

    result = avformat_write_header(afc_output, NULL);

    if (result < 0) {
        LOGE( "Error occurred when opening output file\n");
    }


    LOGE(" INIT OUTPUT SUCCESS !");

    return 1;
}


int VideoClip::addVideoOutputStream(){
    videoOutStream = avformat_new_stream(afc_output , NULL);
    if(videoOutStream == NULL){
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }

    avcodec_parameters_copy(videoOutStream->codecpar , videoStream->codecpar);

    videoOutStream->codecpar->codec_tag = 0 ;

    LOGE(" output video stream id %d " , videoOutStream->codecpar->codec_id );

    if(videoOutStream->codecpar->codec_id == AV_CODEC_ID_NONE){
        LOGE(" video output stream %d " , videoOutStream->codecpar->codec_id );
        return -1;
    }
    videoCodecE  = avcodec_find_encoder(videoOutStream->codecpar->codec_id);
    if( videoCodecE == NULL){
        LOGE(" avcodec_find_encoder FAILD ! ");
        return -1;
    }
    vCtxE = avcodec_alloc_context3(videoCodecE);
    if(vCtxE == NULL){
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }
    if (avcodec_open2(vCtxE, videoCodecE, NULL) < 0) {
        LOGE( "Could not open codec\n");
        return -1;
    }
    LOGE(" VIDEO STREAM WIDTH %d " , videoOutStream->codecpar->width);

    return 1;
}

int VideoClip::addAudioOutputStream(){
    int result = 0;
    audioOutStream = avformat_new_stream(afc_output , NULL);
    if(audioOutStream == NULL){
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    if (afot->audio_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }

    avcodec_parameters_copy(audioOutStream->codecpar , audioStream->codecpar);

    audioOutStream->codecpar->codec_tag = 0 ;

    return 1;
}


void custom_log2(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeglog.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};

void VideoClip::start(){

    int result = -1;
    av_register_all();
    avcodec_register_all();
    av_log_set_callback(custom_log2);
    if(initInput() < 0){
        LOGE(" INIT INPUT FAILD ");
        return ;
    }
    if(initOutput() < 0){
        LOGE(" INIT OUTPUT FAILD ");
        return ;
    }



//    AVPacket *packet = av_packet_alloc();
//    int64_t pts = 0;
//    AVStream *inStream , *outStream;
//    while(true){
//        result = av_read_frame(afc_input , packet);
//        if(result < 0){
//            LOGE(" READ FRAME FAILD ");
//            break;
//        }
//        if(packet->stream_index == video_index){
//            inStream = videoStream;
//            outStream = videoOutStream;
//            pts = (int64_t)(packet->pts * av_q2d(videoStream->time_base) * 1000);
//            //需要解码处理了。
////            LOGE("NALU ? %x , %x , %x , %x " , packet->data[0] , packet->data[1] , packet->data[2] , packet->data[3]);
//        }
//        else if(packet->stream_index == audio_index){
//            inStream = audioStream;
//            outStream = audioOutStream;
//            pts = (int64_t)(packet->pts * av_q2d(audioStream->time_base) * 1000);
//        }
//        if((pts >= startSecond * 1000) && (pts <= endSecond * 1000) ){
//            LOGE(" packet->flags %d " , packet->flags);
//
//
//            packet->pts = av_rescale_q_rnd(packet->pts, inStream->time_base, outStream->time_base, AV_ROUND_NEAR_INF);
//            packet->dts = av_rescale_q_rnd(packet->dts, inStream->time_base, outStream->time_base, AV_ROUND_NEAR_INF);
//            packet->duration = av_rescale_q(packet->duration, inStream->time_base, outStream->time_base);
//            av_interleaved_write_frame(afc_output ,packet);
//            av_packet_unref(packet);
//        }
//    }
//    av_write_trailer(afc_output);
}


VideoClip::~VideoClip(){
    if(afc_input != NULL){
        avformat_close_input(&afc_input);
    }
    if(afc_output != NULL){
        avformat_free_context(afc_output);
    }

}