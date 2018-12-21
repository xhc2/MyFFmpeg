#include <malloc.h>
#include <CallJava.h>
#include "CameraStream.h"
#include <my_log.h>


/**
 * 将视频，和声音录制了，然后再编码处理。
 */
CameraStream::CameraStream() {
    afc_output = NULL;
    outputWidth = 640;
    outputHeight = 480;
    apts = 0;
    vpts = 0;
    aCount = 0;
    vCount = 0;
    initSuccess = false;
    sws = NULL;
    audioStream = NULL;
    videoStream = NULL;
}

int CameraStream::init(const char *url, int width, int height, int pcmsize, CallJava *cj) {
    int ret = avformat_network_init();
    callJava = cj;
    this->inputWidth = width;
    this->inputHeight = height;
    if (ret < 0) {
        callJava->callStr(" avformat_network_init faild !");
        return -1;
    }
    ret = initOutput(url, "flv", &afc_output);
    if (ret < 0) {
        callJava->callStr(" initOutput faild !");
        return -1;
    }
    AVCodecParameters *aparams = avcodec_parameters_alloc();
    aparams->format = AV_SAMPLE_FMT_S16;
    aparams->sample_rate = 44100;
    aparams->channels = 1;
    aparams->channel_layout = AV_CH_LAYOUT_MONO;
    ret = addOutputAudioStream(afc_output, &aCtxE, *aparams);
    LOGE(" ACTXE %d ", aCtxE->frame_size);
    avcodec_parameters_free(&aparams);
    if (ret < 0) {
        callJava->callStr(" addOutputAudioStream faild !");
        return -1;
    }
    audioStream = afc_output->streams[ret];

    AVCodecParameters *vparams = avcodec_parameters_alloc();
    vparams->format = AV_PIX_FMT_YUV420P;
    vparams->width = outputWidth;
    vparams->height = outputHeight;
    ret = addOutputVideoStream(afc_output, &vCtxE, *vparams);
    avcodec_parameters_free(&vparams);
    if (ret < 0) {
        callJava->callStr(" addOutputVideoStream faild !");
        return -1;
    }
    videoStream = afc_output->streams[ret];
    ret = writeOutoutHeader(afc_output, url);
    if (ret < 0) {
        callJava->callStr(" writeOutoutHeader faild !");
        return -1;
    }

    this->start();
    setPause();

    if (width != outputWidth || height != outputHeight) {
        LOGE(" WIDTH %d  ， height %d  , pix %d ", width, height, vCtxE->pix_fmt);
        if (initSwsContext(width, height, vCtxE->pix_fmt)) {
            callJava->callStr(" initSwsContext faild !");
            return -1;
        }
    }

    framePic = av_frame_alloc();
    framePic->width = width;
    framePic->height = height;
    framePic->format = AV_PIX_FMT_YUV420P;

    frameOutV = av_frame_alloc();
    frameOutV->width = outputWidth;
    frameOutV->height = outputHeight;
    frameOutV->format = AV_PIX_FMT_YUV420P;
    ret = av_frame_get_buffer(frameOutV, 0);
    if (ret < 0) {
        callJava->callStr(" av_frame_get_buffer out faild !");
        return -1;
    }

    frameOutA = av_frame_alloc();
    frameOutA->sample_rate = 44100;
    frameOutA->channels = 1;
    frameOutA->channel_layout = AV_CH_LAYOUT_MONO;
    frameOutA->format = AV_SAMPLE_FMT_S16;
    frameOutA->nb_samples = aCtxE->frame_size;


    aCalDuration = AV_TIME_BASE / 44100;
    vCalDuration = AV_TIME_BASE / outFrameRate;
    initSuccess = true;

    return 1;
}


int CameraStream::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outputWidth, outputHeight,
                         AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {
        return -1;
    }
    return 1;
}

void CameraStream::destroySwsContext() {
    if (sws != NULL) {
        sws_freeContext(sws);
        sws = NULL;
    }
}


int CameraStream::startRecord() {
    setPlay();
    return 1;
}

int CameraStream::pauseRecord() {
    setPause();
    return 1;
}


void CameraStream::pushAudioStream(jbyte *pcm, int size) {
    if (pause || isExit || !initSuccess) {
        return;
    }
    frameOutA->data[0] = (uint8_t *) pcm;
    frameOutA->linesize[0] = size;
    frameOutA->pts = aCount * aCalDuration;
    aCount += size / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

    AVPacket *pkt = encodeFrame(frameOutA, aCtxE);
    if (pkt != NULL) {
        pkt->stream_index = audioStream->index;
        audioPktQue.push(pkt);
    }

}

void CameraStream::pushVideoStream(jbyte *yuv) {
    if (pause || isExit || !initSuccess) {
        return;
    }

    framePic->data[0] = (uint8_t *) (yuv);
    framePic->data[1] = (uint8_t *) (yuv + inputWidth * inputHeight * 5 / 4);
    framePic->data[2] = (uint8_t *) (yuv + inputWidth * inputHeight);
    //修改分辨率统一输出大小
    AVPacket *pkt = NULL;
    if (sws != NULL) {
        LOGE(" sws_scale ");
        sws_scale(sws, (const uint8_t *const *) framePic->data, framePic->linesize,
                  0, inputHeight, frameOutV->data, frameOutV->linesize);
        frameOutV->pts = vCount * vCalDuration;
        pkt = encodeFrame(frameOutV, vCtxE);
    } else {
        framePic->linesize[0] = outputWidth;
        framePic->linesize[1] = outputWidth / 2;
        framePic->linesize[2] = outputWidth / 2;
        framePic->pts = vCount * vCalDuration;
        pkt = encodeFrame(framePic, vCtxE);
    }
    vCount++;
    if (pkt != NULL) {
        pkt->stream_index = videoStream->index;
        videoPktQue.push(pkt);
    }
}


//新开一个线程来混合
void CameraStream::run() {
    while (!isExit) {
        if (pause) {
            threadSleep(3);
            continue;
        }
        if (videoPktQue.size() <= 0 || audioPktQue.size() <= 0
            || audioStream == NULL ||  videoStream == NULL) {
            continue;
        }
        LOGE(" VIDEO SIZE %d ,  AUDIO size  %d " , videoPktQue.size() , audioPktQue.size());
        AVPacket *aPkt = audioPktQue.front();
        AVPacket *vPkt = videoPktQue.front();
        if (av_compare_ts(apts, audioStream->time_base, vpts, videoStream->time_base) < 0) {
            av_packet_rescale_ts(aPkt, timeBaseFFmpeg, audioStream->time_base);
            apts = aPkt->pts;
            av_interleaved_write_frame(afc_output, aPkt);
            audioPktQue.pop();
        } else {
            av_packet_rescale_ts(vPkt, timeBaseFFmpeg, videoStream->time_base);
            vpts = vPkt->pts;
            av_interleaved_write_frame(afc_output, vPkt);
            videoPktQue.pop();
        }
    }
}


CameraStream::~CameraStream() {
    this->stop();
    this->join();
    while(!videoPktQue.empty()){
        AVPacket *pkt = videoPktQue.front();
        if(pkt != NULL ){
            av_packet_free(&pkt);
        }
        videoPktQue.pop();
    }
    while(!audioPktQue.empty()){
        AVPacket *pkt = audioPktQue.front();
        if(pkt != NULL ){
            av_packet_free(&pkt);
        }
        audioPktQue.pop();
    }
    if(afc_output != NULL && afc_output->oformat != NULL && afc_output->pb != NULL ){
        av_write_trailer(afc_output);
    }

    destroySwsContext();
    if(aCtxE != NULL){
        avcodec_free_context(&aCtxE);
    }
    if(vCtxE != NULL){
        avcodec_free_context(&vCtxE);
    }
    if(afc_output != NULL){
        avformat_free_context(afc_output);
    }
    if(framePic != NULL ){
        av_frame_free(&framePic);
    }
    if(frameOutV != NULL ){
        av_frame_free(&frameOutV);
    }
    if(frameOutA != NULL ){
        av_frame_free(&frameOutA);
    }

    if(callJava != NULL){
        delete callJava;
    }


    LOGE(" DESTROY CAMERA !! ");
}