//
// Created by Administrator on 2018/10/25/025.
//
#include "VideoJoint.h"

/**
 * 拼接所有不一定的视频都是相同的分辨率
 * 所以所有输入的视频都需要把分辨率转成相同的。
 * 流程
 * 1.输出先确定（视频流，输出流）
 * 2.再确定输入（视频流，输出流）
 * 3.初始化音频重采样
 * 4.初始化视频裁剪器
 * 5.输入的音频需要重采样
 * 6.输入的视频需要重改分辨率
 * 7.编码完成后到2
 */

VideoJoint::VideoJoint(vector<char *> inputPath  , const char *output , int outWidth , int outHeight){
    this->outWidth = outWidth;
    this->outHeight = outHeight;
    this->inputPaths = inputPath;

    int pathLen = strlen(output);
    pathLen++;
    this->outPath = (char *)malloc(pathLen);
    strcpy(this->outPath , output);
    LOGE(" OUTPUT path %s " , this->outPath );
}


void VideoJoint::startJoint(){
    av_register_all();
    avcodec_register_all();
#ifdef DEBUG
    av_log_set_callback(custom_log);
#endif
    int result = initOutput(this->outPath);
    if(result < 0){
        LOGE(" init output faild !");
        return ;
    }
    for(int i = 0 ; i < inputPaths.size() ; ++i){
        destroyInput();

        result = initInput(inputPaths.at(i));
        if(result < 0){
            LOGE(" initinput faild !");
            return ;
        }

    }
}



int VideoJoint::initInput(char *path){
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

    for (int i = 0; i < afc_input->nb_streams; ++i) {
        AVStream *stream = afc_input->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
            audioCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        }
    }
//videoCodecD
    if (videoCodecD == NULL) {
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    vCtxD = avcodec_alloc_context3(videoCodecD);

    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }

    avcodec_parameters_to_context(vCtxD, afc_input->streams[video_index]->codecpar);

    result = avcodec_open2(vCtxD, videoCodecD, NULL);

    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }
//decoder audio
    if(audioCodecD == NULL){
        LOGE(" audioCodecD find faild !");
        return -1;
    }
    aCtxD = avcodec_alloc_context3(audioCodecD);

    if (!aCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }
    avcodec_parameters_to_context(aCtxD, afc_input->streams[audio_index]->codecpar);
    result = avcodec_open2(aCtxD, audioCodecD, NULL);

    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }

    LOGE(" init input success ");

    return 1;
}

void VideoJoint::destroyInput(){

    if(vCtxD != NULL){
        avcodec_free_context(&vCtxD);
        vCtxD = NULL;
    }
    if(aCtxD != NULL){
        avcodec_free_context(&aCtxD);
        aCtxD = NULL;
    }
    if(afc_input != NULL){
        avformat_free_context(afc_input);
        afc_input = NULL;
    }
}

void VideoJoint::destroyOutput(){
    if(vCtxE != NULL){
        avcodec_free_context(&vCtxE);
        vCtxE = NULL;
    }
    if(aCtxE != NULL){
        avcodec_free_context(&aCtxE);
        aCtxE = NULL;
    }
    if(afc_output != NULL){
        avformat_free_context(afc_output);
        afc_output = NULL;
    }
}

int VideoJoint::addVideoOutputStream(int width, int height) {
    int result = 0;
    videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }

    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    videoCodecE = avcodec_find_encoder(afot->video_codec);

    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return -1;
    }
    vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }

    vCtxE->bit_rate = 400000;
    vCtxE->time_base = (AVRational) {1, 25};
    vCtxE->framerate = (AVRational) {25, 1};
    vCtxE->gop_size = 10;
    vCtxE->max_b_frames = 1;
    vCtxE->pix_fmt = AV_PIX_FMT_YUV420P;
    vCtxE->codec_type = AVMEDIA_TYPE_VIDEO;
    vCtxE->width = width;
    vCtxE->height = height;

    result = avcodec_parameters_from_context(videoOutStream->codecpar, vCtxE);

    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }

    result = avcodec_open2(vCtxE, videoCodecE, NULL);

    if (result < 0) {
        LOGE( "video Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO !");

    return 1;
}

int VideoJoint::addAudioOutputStream() {
    int result = 0;
    audioOutStream = avformat_new_stream(afc_output, NULL);
    if (audioOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }

    if (afot->audio_codec == AV_CODEC_ID_NONE) { //86018
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    LOGE(" AUDIO encode %d " , afot->audio_codec );
//    afot->audio_codec = AV_CODEC_ID_AAC;
    audioCodecE = avcodec_find_encoder(afot->audio_codec);
    if(audioCodecE != NULL){
        LOGE(" audioCodecE NULL ");
        return -1;
    }
    aCtxE = avcodec_alloc_context3(audioCodecE);
    if(aCtxE == NULL){
        LOGE("AUDIO avcodec_alloc_context3 FAILD !");
        return -1;
    }

    aCtxE->bit_rate = 64000;
    aCtxE->sample_fmt = AV_SAMPLE_FMT_S16;
    aCtxE->sample_rate    = 44100;
    aCtxE->channel_layout =  AV_CH_LAYOUT_MONO;
    aCtxE->channels       = av_get_channel_layout_nb_channels(aCtxE->channel_layout);
    result = avcodec_open2(vCtxE, videoCodecE, NULL);
    if (result < 0) {
        LOGE(" audio Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO  !");
    return 1;
}




int VideoJoint::initOutput(char *path){
    int result = 0;
    afc_output = NULL;
//    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, path);
    afc_output = avformat_alloc_context();
    afc_output->oformat = av_guess_format(NULL, path, NULL);
    afot = afc_output->oformat;
    LOGE(" outformat name %s  " ,afc_output->oformat->name );
    LOGE(" VIDEO CODEC %d " , afot->video_codec);
    LOGE(" AUDIO CODEC %d " , afot->audio_codec);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    if (addVideoOutputStream(outWidth, outHeight) < 0) {
        return -1;
    }
    if (addAudioOutputStream() < 0) {
        return -1;
    }

    if (!(afot->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc_output->pb, path, AVIO_FLAG_WRITE);
        if (result < 0) {
            LOGE("Could not open output file %s ", path);
            return -1;
        }
    }

    result = avformat_write_header(afc_output, NULL);

    if (result < 0) {
        LOGE(" avformat_write_header %s", av_err2str(result));
    }

    LOGE(" INIT OUTPUT SUCCESS !");

    return 1;
}


VideoJoint::~VideoJoint(){
    destroyOutput();
    destroyInput();
    //还需释放其他路径等
}