//
// Created by dugang on 2018/7/31.
//

#include <my_log.h>
#include "PublishStream.h"

PublishStream::PublishStream(const char* url , const char* inpath){
    this->url = url;
    this->inpath = inpath;
    ofmt = NULL;
    ifmtCtx = NULL;
    ofmtCtx = NULL;;
    pkt = NULL;;

    av_register_all();
    avformat_network_init();
    int result = 0;
    result = avformat_open_input(&ifmtCtx , inpath , 0 , 0 );
    if(result < 0){
        LOGE(" input error %s " , av_err2str(result));
        return ;
    }



}


PublishStream::~PublishStream(){

}