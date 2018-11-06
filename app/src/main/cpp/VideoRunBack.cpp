/**
 * 倒放
 * 视频部分：
 * 找到一个gop然后顺序解码
 * 一个解码的gop
 * 然后写入yuv文件中，然后倒序读入中进行编码
 * 音频部分：
 * 直接写入队列中缓存队列中。
 *
 * 输出部分尽量和原文件保持一致（采样率，声道，之类的）
 *
 */

#include <my_log.h>
#include "VideoRunBack.h"

VideoRunBack::VideoRunBack(const char* path ,const char* outPath){

    int inputLen = strlen(path);
    inputLen ++;
    this->inputPath = (char *)malloc(inputLen);
    strcpy(this->inputPath , path);

    int len = strlen(outPath);
    len++;
    this->outPath = (char *)malloc(len);
    strcpy(this->outPath , outPath);
    initValue();

}

void VideoRunBack::initValue(){
    afc_output = NULL;
    afc_input = NULL;
    videoCodecD = NULL;
    audioIndexInput = -1;
    videoIndexInput = -1;
    videoIndexOutput = -1;
    audioIndexOutput = -1;
    outFrameRate = 25;
    afot = NULL;
    audioOutStream = NULL;
    videoOutStream = NULL;
    gopFrameCount = 0;
    inputDuration = 0 ;
    gopDuration = 0;
    gopCount = 0 ;
}


int VideoRunBack::initInput(){
    int result ;
    result = avformat_open_input(&afc_input, inputPath, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return -1;
    }
    result = avformat_find_stream_info(afc_input, 0);
    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return -1;
    }
    for (int i = 0; i < afc_input->nb_streams; ++i) {
        AVStream *stream = afc_input->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndexInput = i;
            videoinputStream = stream;
            inWidth = stream->codecpar->width;
            inHeight = stream->codecpar->height;
            LOGE(" inwidth %d , inheight %d " , inWidth , inHeight);
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndexInput = i;
            audioinputStream = stream;
        }
    }

//  videoCodecD
    if (videoCodecD == NULL) {
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    vCtxD = avcodec_alloc_context3(videoCodecD);
    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }
    result = avcodec_parameters_to_context(vCtxD, afc_input->streams[videoIndexInput]->codecpar);
    if (result < 0) {
        LOGE(" avcodec_parameters_to_context Faild !");
        return -1;
    }
    result = avcodec_open2(vCtxD, videoCodecD, NULL);
    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }

    gopSize = vCtxD->gop_size;
    inputDuration = afc_input->duration;
    gopDuration = (int64_t)(((float)gopSize / (float)videoinputStream->nb_frames) * inputDuration);
    if(gopDuration <= 0 ){
        LOGE(" GOP DURATION FAILD ! gopsize  %d , duration %lld , gopDuration %lld " , gopSize , inputDuration , gopDuration);
        return -1;
    }
    LOGE(" inputduration %lld , gopDuration %lld " ,inputDuration ,  gopDuration);
    return 1;
}

int VideoRunBack::initOutput() {
    int result ;
    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, outPath);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    result = addVideoOutputStream(inWidth , inHeight);
    if(result < 0){
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    result = addAudioOutputStream();
    if(result < 0){
        LOGE(" addAudioOutputStream ");
        return -1;
    }



    return 1;
}


int VideoRunBack::addVideoOutputStream(int width , int height){
    int result = 0;
    videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    videoIndexOutput = videoOutStream->index;
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    videoCodecE = avcodec_find_encoder(afot->video_codec);
    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return -1;
    }
    LOGE("video ENCODE NAME %s ", videoCodecE->name);
    vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }

    vCtxE->bit_rate = videoinputStream->codecpar->bit_rate;
    vCtxE->time_base = (AVRational) {1, outFrameRate};
    vCtxE->framerate = (AVRational) {outFrameRate, 1};
    vCtxE->gop_size = gopSize;
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
        LOGE("video Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO !");

    return 1;
}

void VideoRunBack::run(){


}

int VideoRunBack::addAudioOutputStream(){

    return 1;
}

AVFrame *VideoRunBack::deocdePacket(AVPacket *packet, AVCodecContext *decode) {

    int result = avcodec_send_packet(decode, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(decode, frame);
        if (result < 0) {
            LOGE(" avcodec_receive_frame  faild %s ", av_err2str(result));
            av_frame_free(&frame);
            return NULL;
        }
        return frame;
    }
    av_frame_free(&frame);
    return NULL;
}


AVPacket *VideoRunBack::encodeFrame(AVFrame *frame, AVCodecContext *encode) {
    if(frame == NULL || encode == NULL){
        return NULL;
    }
    int result = 0;
    result = avcodec_send_frame(encode, frame);
    if (result < 0) {
        LOGE(" avcodec_send_frame faild ! %s ", av_err2str(result));
        return NULL;
    }
    AVPacket *packet = av_packet_alloc();
    while (result >= 0) {
        result = avcodec_receive_packet(encode, packet);
        if (result < 0) {
            LOGE(" avcodec_receive_packet faild ! %s ", av_err2str(result));
            av_packet_free(&packet);
            return NULL;
        }
        return packet;
    }
    av_packet_free(&packet);
    return NULL;
}


int VideoRunBack::startBackParse(){
    LOGE(" ------------------------------------------- " );
    av_register_all();
#ifdef DEBUG
    av_log_set_callback(custom_log);
#endif
    int result = 0;
    char *tempYuv = "sdcard/FFmpeg/temp.yuv";
    FILE *fCache = fopen(tempYuv ,"wb+");
    FILE *testFile = fopen("sdcard/FFmpeg/test.yuv" , "wb");
    result = initInput();
    if(result < 0){
        LOGE(" initInput faild ! ");
        return -1;
    }
    result = initOutput();
    if(result < 0){
        LOGE(" initOutput faild ! ");
        return -1;
    }
    int yuvSize = inWidth * inHeight * 3 / 2;
    char *readBuffer = (char *)malloc(yuvSize);
    AVPacket *pkt = av_packet_alloc();

    result = av_seek_frame(afc_input , -1 , inputDuration , AVSEEK_FLAG_BACKWARD  );
    if(result < 0){
        LOGE(" av_seek_frame %s " , av_err2str(result));
        return  -1;
    }
    int64_t firstGopPts = 0;
    //test
    while(true){
        result = av_read_frame(afc_input, pkt);
        if(result < 0){
            LOGE(" av_read_frame FAILD %s " , av_err2str(result));
            break;
        }
        if(pkt->stream_index == audioIndexInput) {
            continue;
        }
        if(pkt->stream_index == videoIndexInput) {
            LOGE(" PTS %lld " , pkt->pts);
            gopFrameCount ++;
            if(firstGopPts == 0){
                firstGopPts = av_rescale_q(pkt->pts , videoinputStream->time_base , (AVRational){1 , AV_TIME_BASE});
                LOGE(" FIRST PTS %lld " , firstGopPts);
            }
        }
        if(gopFrameCount % gopSize == 0){
            LOGE(" gopFrameCount %d " , gopFrameCount);
            gopCount ++;
            av_seek_frame(afc_input , -1 , firstGopPts - gopDuration / 2  , AVSEEK_FLAG_BACKWARD  );
            firstGopPts = 0;
            LOGE(" \n\n\n ");
        }
        if(gopFrameCount >= videoinputStream->nb_frames){
            break;
        }
    }


//    while(true){
//
//        result = av_read_frame(afc_input, pkt);
//        if(result < 0){
//            LOGE(" av_read_frame FAILD %s " , av_err2str(result));
//            break;
//        }
//        if(pkt->stream_index == audioIndexInput) {
//            continue;
//        }
//        if(pkt->stream_index == videoIndexInput){
//            gopFrameCount ++;
//            LOGE(" PTS %lld " , pkt->pts);
////            LOGE(" gopFrameCount %d " , gopFrameCount);
//        }
//        AVFrame *vFrame = deocdePacket(pkt , vCtxD);
//        if(vFrame != NULL){
//            writeFrame2File(vFrame , fCache);
//            av_frame_free(&vFrame);
//        }
//        if((gopFrameCount % gopSize ) == 0){
//            gopCount ++;
//            result = av_seek_frame(afc_input , videoIndexInput , inputDuration - (gopCount * gopDuration) , AVSEEK_FLAG_BACKWARD  );
//            if(result < 0){
//                LOGE(" av_seek_frame %s " , av_err2str(result));
//                break;
//            }
//            int frameCount = gopSize;
//            while(true){
//                //清空其中的数据
//                vFrame = deocdePacket(NULL , vCtxD);
//                if(vFrame == NULL){
//                    avformat_flush(afc_input);
//                    break;
//                }
//                else{
//                    writeFrame2File(vFrame , fCache);
//                }
//            }
//            fflush(fCache);
//            //这里开始倒序读取
//            while(true){
//                frameCount --;
//                fseek(fCache , yuvSize * frameCount , SEEK_SET);
//                fread(readBuffer , 1 , yuvSize , fCache);
//                fwrite(readBuffer , 1 , yuvSize , testFile);
//                LOGE(" frameCount %d " , frameCount);
//
//                if(frameCount == 0){
//                    break;
//                }
//            }
//            //重开
//            fclose(fCache);
//            fCache = fopen(tempYuv , "wb+");
//
//        }
//        if(gopFrameCount >= videoinputStream->nb_frames){
//            break;
//        }
//    }
    LOGE(" END ");
    return 1;
}
void VideoRunBack::writeFrame2File(AVFrame *vFrame , FILE *file){
    fwrite(vFrame->data[0] , 1 , vFrame->linesize[0] * inHeight , file);
    fwrite(vFrame->data[1] , 1 , vFrame->linesize[1] * inHeight / 2 , file);
    fwrite(vFrame->data[2] , 1 , vFrame->linesize[2] * inHeight / 2, file);
}


VideoRunBack::~VideoRunBack(){

}