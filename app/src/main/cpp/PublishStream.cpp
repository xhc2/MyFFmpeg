//
// Created by dugang on 2018/7/31.
//
/**
 * 根据官方文档 http://ffmpeg.org/doxygen/3.4/remuxing_8c-example.html#a37
 */
#include <my_log.h>
#include "PublishStream.h"

PublishStream::PublishStream(const char* url , const char* inpath , CallJava *cj ){
    this->url = url;
    this->inpath = inpath;
    LOGE("URL %s , inpath %s "  ,url , inpath);
    ofmt = NULL;
    ifmtCtx = NULL;
    ofmtCtx = NULL;
    pkt = NULL;
    this->cj = cj;
    isExist = false;
    av_register_all();
    avformat_network_init();
    int result = 0;
    result = avformat_open_input(&ifmtCtx , inpath , 0 , 0 );
    if(result < 0){
        LOGE(" input error %s " , av_err2str(result));
        return ;
    }
    result = avformat_find_stream_info(ifmtCtx, 0);
    if (result < 0) {
        cj->callStr("Failed to retrieve input stream information");
        LOGE( "Failed to retrieve input stream information");
        return ;
    }
    //目前rtmp flv 有点用，其他格式都有点问题。
    result = avformat_alloc_output_context2(&ofmtCtx, NULL, "flv" , url); //RTMP
    if (result < 0) {
        cj->callStr( "avformat_alloc_output_context2 faild");
        LOGE( "avformat_alloc_output_context2 faild");
        return ;
    }
    if (ofmtCtx == NULL) {
        cj->callStr( "faild create output context\n");
        LOGE( "faild create output context\n");
        return ;
    }
    ofmt = ofmtCtx ->oformat;

    for(int i = 0 ; i < ifmtCtx->nb_streams ; ++ i){
        AVStream *as = ifmtCtx->streams[i];

        //只需要音频，视频流。
        if (as->codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
                as->codecpar->codec_type != AVMEDIA_TYPE_VIDEO  ) {
            continue;
        }
        if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            audioIndex = i;
        }
        else if( as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ){
            videoIndex = i;
        }

        AVStream *os = avformat_new_stream(ofmtCtx , NULL);
        if(os == NULL){
            cj->callStr( "CREATE NEW STREAM FAILD ");
            LOGE(" CREATE NEW STREAM FAILD ");
            return ;
        }
        result = avcodec_parameters_copy(os->codecpar, as->codecpar);
        if(result <0 ){
            cj->callStr( " avcodec_parameters_copy faild ");
            LOGE( "avcodec_parameters_copy faild %s " , as->codecpar->format);
            return ;
        }
        as->codecpar->codec_tag = 0 ;
    }

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        result = avio_open(&ofmtCtx->pb, url, AVIO_FLAG_WRITE);
        if (result < 0) {
            cj->callStr( " Could not open output file");
            LOGE("Could not open output file '%s'  , %s ", url , av_err2str(result));
            return ;
        }
    }

    result = avformat_write_header(ofmtCtx , NULL);
    if(result < 0){
        cj->callStr( " avformat_write_header faild");
        LOGE( "avformat_write_header faild %s " , av_err2str(result) );
        return ;
    }

    /**
     * 这个一口气往外写数据，会不会有丢帧的情况。极端情况，你一口气丢了10G出去，
     * 然后别人才播放了20s的。那数据是肯定会丢的吧？
     */
    while(!isExist){
//        av_usleep(1000 * 25);
        pkt = av_packet_alloc();
        result = av_read_frame(ifmtCtx , pkt);
        if(result < 0){
//            cj->callStr( " av_read_frame FAILD ");
            LOGE(" av_read_frame FAILD ");
            av_packet_free(&pkt);
            break;
        }
        result = av_interleaved_write_frame(ofmtCtx , pkt);
        if(result < 0){
//            cj->callStr( " av_interleaved_write_frame FAILD  ");
            LOGE(" av_interleaved_write_frame FAILD ");
            break;
        }
    }
    av_write_trailer(ofmtCtx);
    LOGE(" SUCCESS ");
}


PublishStream::~PublishStream(){
    LOGE(" DESTROY STREAM ");
    avformat_close_input(&ifmtCtx);
    if (ofmtCtx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmtCtx->pb);
    avformat_free_context(ofmtCtx);
    isExist = true;
}