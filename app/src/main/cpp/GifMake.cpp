//
// Created by Administrator on 2018/12/5/005.
//

#include <my_log.h>
#include "GifMake.h"

GifMake::GifMake(const char *inputPath, const char *outPath) {
    isExit = false;
    frameCount = 0 ;
    outFormat =  AV_PIX_FMT_RGB8 ;
    int result = buildInput(inputPath);
    if(result < 0){
        LOGE(" open input faild !");
        return ;
    }
    outWidth =  vCtxD->width ;
    outHeight = vCtxD->height ;
    result = buildOutput(outPath);
    if(result < 0){
        LOGE(" buildOutput faild !");
        return ;
    }
    LOGE("out width %d , outheight %d " , outWidth , outHeight);
    result = initSwsContext(vCtxD->width , vCtxD->height , vCtxD->pix_fmt);
    if(result < 0){
        LOGE(" initSwsContext faild !");
        return ;
    }


}



int GifMake::buildOutput(const char* outPath ) {

    int result ;
    afc_output = NULL;
    result = initOutput(outPath , &afc_output);
    if(result < 0 ){
        LOGE(" initOutput faild !");
        return -1;
    }

    AVCodecParameters *codecpar = new AVCodecParameters();
    codecpar->width = outWidth ;
    codecpar->height = outHeight ;
    codecpar->format = outFormat ;

    result = addOutputVideoStream(afc_output, &vCtxE, *codecpar);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    videoOutputStreamIndex = result;
    result = writeOutoutHeader(afc_output,outPath);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    vCalDuration =   AV_TIME_BASE / outFrameRate;
#ifdef DEBUG
    av_dump_format(afc_output, 0, outPath, 1);
#endif
//    AVStream *videoOutStream = avformat_new_stream(afc_output, NULL);
//    if (videoOutStream == NULL) {
//        LOGE(" VIDEO STREAM NULL ");
//        return -1;
//    }
//    if (afc_output->oformat->video_codec == AV_CODEC_ID_NONE) {
//        LOGE(" VIDEO AV_CODEC_ID_NONE ");
//        return -1;
//    }
//    AVCodec *videoCodecE = avcodec_find_encoder(afc_output->oformat->video_codec);
//    if (videoCodecE == NULL) {
//        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
//        return -1;
//    }
//    LOGE("video ENCODE NAME %s ", videoCodecE->name);
//    int outFrameRate = 20;
//    vCtxE = avcodec_alloc_context3(videoCodecE);
//    if(vCtxE == NULL){
//        LOGE("avcodec_alloc_context3 FAILD ! ");
//        return -1 ;
//    }
//    vCtxE->bit_rate= outWidth * outHeight * 3 / 2 * outFrameRate ;
//    vCtxE->framerate = (AVRational) {outFrameRate, 1};
//    vCtxE->time_base = (AVRational) {1, outFrameRate};
//    vCtxE->pix_fmt = outFormat;
//    vCtxE->gop_size = 20;
//    vCtxE->codec_type = AVMEDIA_TYPE_VIDEO;
//    vCtxE->width = outWidth ;
//    vCtxE->height = outHeight ;
//    if (vCtxE == NULL) {
//        LOGE(" avcodec_alloc_context3 FAILD ! ");
//        return -1;
//    }
//    result = avcodec_parameters_from_context(videoOutStream->codecpar, vCtxE);
//    if (result < 0) {
//        LOGE(" avcodec_parameters_from_context FAILD ! ");
//        return -1;
//    }
//
//    result = avcodec_open2(vCtxE, videoCodecE, NULL);
//
//    if (result < 0) {
//        LOGE("video Could not open codec %s ", av_err2str(result));
//        return -1;
//    }
//
//
//    result = writeOutoutHeader(afc_output , outPath);
//    if (result < 0) {
//        LOGE(" writeOutoutHeader faild %s ", av_err2str(result));
//        return -1;
//    }
//    videoStreamOutputIndex = videoOutStream->index;
    LOGE(" VIDEO OUTPUT STREAM %d  " , videoOutputStreamIndex);
    LOGE(" INIT OUTPUT SUCCESS GIF !");
    return 1;
}


int GifMake::buildInput(const char *inputPath ) {
    afc_input = NULL;
    int result = open_input_file(inputPath , &afc_input);
    if(result < 0){
        LOGE(" open_input_file faild  ");
        return -1;
    }
    result = getVideoDecodeContext(afc_input , &vCtxD);
    if(result < 0){
        LOGE(" getVideoDecodeContext faild  ");
        return -1;
    }
    videoStreamIndex = result ;
    return 1;
}

int GifMake::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outWidth, outHeight,
                         outFormat, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {

        return -1;
    }
    return 1;
}

void GifMake::destroySwsContext() {
    if (sws != NULL) {
        sws_freeContext(sws);
        sws = NULL;
    }
}


int GifMake::startParse() {
    int result ;
    FILE *rgbF = fopen("sdcard/FFmpeg/test.rgb8" , "wb+");
    AVPacket *pkt = av_packet_alloc();
//    AVFrame *outVFrame = av_frame_alloc();
//    outVFrame->width = outWidth;
//    outVFrame->height = outHeight;
//    outVFrame->format = outFormat;
//    result = av_frame_get_buffer(outVFrame, 0);
//    if (result < 0) {
//        LOGE(" av_frame_get_buffer FAILD ! ");
//        return -1;
//    }
//    result = av_frame_make_writable(outVFrame);
//    if (result < 0) {
//        LOGE(" av_frame_get_buffer FAILD ! ");
//        return -1;
//    }
//    result = av_seek_frame(afc_input , -1 ,  ((float) 10 / 1000) * AV_TIME_BASE * afc_input->start_time , AVSEEK_FLAG_BACKWARD);
//    if (result < 0) {
//        LOGE(" av_seek_frame FAILD ! ");
//        return -1;
//    }
    while(!isExit){
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            LOGE(" ************* startDecode av_read_frame FAILD ! %s ", av_err2str(result));
            break;
        }
        AVFrame *frame = NULL;
        if (pkt->stream_index == videoStreamIndex) {
            frame = decodePacket(vCtxD, pkt);
            if (frame != NULL) {
                AVFrame *outVFrame = av_frame_alloc();
                outVFrame->width = outWidth;
                outVFrame->height = outHeight;
                outVFrame->format = outFormat;
                  av_frame_get_buffer(outVFrame, 0);
                sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                          0, frame->height, outVFrame->data, outVFrame->linesize);
//                fwrite(outVFrame->data[0] , 1 , outVFrame->linesize[videoOutputStreamIndex] * outHeight  ,rgbF );
                outVFrame->pts = frameCount * vCalDuration;
                frameCount++;
//                LOGE(" outvFrame %lld " , outVFrame->pts);
                av_frame_free(&frame);
                AVPacket *vPkt = encodeFrame(outVFrame, vCtxE);
                av_frame_free(&outVFrame);
                if (vPkt != NULL) {
                    LOGE(" WIRTE GIF FRAME %lld  , size %d " , vPkt->pts , vPkt->size);
                    av_packet_rescale_ts(vPkt, timeBaseFFmpeg, afc_output->streams[videoOutputStreamIndex]->time_base);
                    vPkt->stream_index = 0;
                    result = av_write_frame(afc_output , vPkt);

                    av_packet_free(&vPkt);
                    if(result < 0){
                        LOGE(" av_write_frame %s " , av_err2str(result));
                        break;
                    }
                }
            }
        }
    }
    LOGE(" END ");
    writeTrail(afc_output);
    return 1;
}

GifMake::~GifMake() {
    isExit = true;
    destroySwsContext();
}


