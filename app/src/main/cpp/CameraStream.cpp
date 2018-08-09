
#include <malloc.h>

#include <CallJava.h>
#include "CameraStream.h"
#include <my_log.h>

CameraStream::CameraStream(const char * url , int width , int height , CallJava *cj ){
    afc = NULL;
    afot = NULL;
    yuv = NULL;
    count = 0;
    pixFmt = AV_PIX_FMT_YUV420P;
    this->url = url;
    this->width = width;
    this->height = height;
    LOGE(" path %s , width = %d , height = %d " , url , width , height);
    this->size = (int)(width * height * 1.5f);
    this->cj = cj;
    yuv = (char *) malloc(size * sizeof(char));
    if(yuv == NULL){
        cj->callStr("YUV ALLOC FAILD2 !");
        return ;
    }
//    fileU = fopen("sdcard/FFmpeg/yuv.u" , "wb+");
//    fileV = fopen("sdcard/FFmpeg/yuv.v" , "wb+");
    initFFmpeg();
    LOGE(" YUV ALLOC success !");
}

void custom_log(void *ptr, int level, const char* fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeglog.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};
void CameraStream::initFFmpeg(){
    int result = 0;
    av_register_all();
    avformat_network_init();
    av_log_set_callback(custom_log);
    result = avformat_alloc_output_context2(&afc, NULL, "flv" , url);
    if(result < 0 || afc == NULL ){
        cj->callStr(" avformat_alloc_output_context2 faild ");
        return ;
    }
    afot = afc->oformat;
    os = avformat_new_stream(afc , NULL);
    if(os == NULL){
        cj->callStr( "CREATE NEW STREAM FAILD ");
        return ;
    }
    os->time_base = (AVRational){1, 25};
    LOGE(" NB STREAM %d " ,afc->nb_streams );
    os->id = afc->nb_streams - 1;

    LOGE(" VIDEO CODE %d " , afot->video_codec);

    if(afot->video_codec == AV_CODEC_ID_NONE){
        cj->callStr( " VIDEO AV_CODEC_ID_NONE ");
        return ;
    }

    AVCodec *vCode = avcodec_find_encoder(afot->video_codec);

    if(vCode == NULL ){
        cj->callStr( " avcodec_find_video_encoder faild ! " );
        return ;
    }

    vCodeCtx = avcodec_alloc_context3(vCode);
    if(vCodeCtx == NULL){
        cj->callStr( " vcode context faild ! " );
        return ;
    }
    vCodeCtx->width = width;
    vCodeCtx->height = height;
    vCodeCtx->gop_size = 20;
    vCodeCtx->pix_fmt = pixFmt;
    vCodeCtx->codec_id = afot->video_codec;
    vCodeCtx->bit_rate = 400000;
    vCodeCtx->time_base = (AVRational){1, 25};
    vCodeCtx->framerate = (AVRational){25, 1};
//    vCodeCtx->thread_count = 4;
    os->codec = vCodeCtx;
    LOGE(" stream time base den %d , num %d " , os->time_base.den , os->time_base.num);
    result = avcodec_parameters_from_context(os->codecpar , vCodeCtx);
    if(result < 0){
        cj->callStr( " avcodec_parameters_from_context faild " );
        return ;
    }

    result = avcodec_open2(vCodeCtx , NULL , NULL);

    if(result < 0){
        cj->callStr( "avcodec_open2 faild ! " );
        return ;
    }
    framePic = av_frame_alloc();
    framePic->format = pixFmt;
    framePic->width = width;
    framePic->height = height;
    result = av_frame_get_buffer(framePic , 0);
    if(result < 0){
        cj->callStr( "av_frame_get_buffer faild ! " );
        return ;
    }
    result = av_frame_make_writable(framePic);
    if(result < 0){
        cj->callStr( "av_frame_make_writable faild ! " );
        return ;
    }
    LOGE(" linesize[0] %d , linesize[1] %d , linesize[2] %d " ,
         framePic->linesize[0] , framePic->linesize[1] , framePic->linesize[2]);

    if (!(afc->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc->pb, url, AVIO_FLAG_WRITE);
        if (result < 0) {
            cj->callStr("Could not open output file  " );
            return ;
        }
    }

    if (afc->oformat->flags & AVFMT_GLOBALHEADER)
        vCodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    result = avformat_write_header(afc, NULL);
    if(result < 0){
        cj->callStr( " avformat_write_header faild ! " );
        return;
    }

    LOGE(" FFMPEG SUCCESS ! ");
}



void CameraStream::pushStream(jbyte *yuv){

    memcpy(this->yuv , yuv ,  size);

    //y
    framePic->data[0] = (uint8_t*)(this->yuv);
    //u
    framePic->data[1] = (uint8_t*)(this->yuv + width * height * 5 / 4 ) ;
    //v
    framePic->data[2] = (uint8_t*)(this->yuv + width * height) ;

    framePic->pts = count ++;

    int ret = avcodec_send_frame(os->codec, framePic);
    if ( ret < 0) {
        LOGE(" Error sending a frame for encoding ");
        return;
    }

    while (ret >= 0) {
        AVPacket *pkt = av_packet_alloc();
        ret = avcodec_receive_packet(os->codec, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            av_packet_free(&pkt);
            return;
        }

        else if (ret < 0) {
            av_packet_free(&pkt);
            return;
        }
        av_write_frame(afc , pkt);
        av_packet_free(&pkt);
    }
}


CameraStream::~CameraStream(){
    LOGE(" DESTROY av_write_trailer ");
    av_write_trailer(afc);
}