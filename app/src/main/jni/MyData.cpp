//
// Created by dugang on 2018/7/4.
//
#include "my_data.h"

void MyData::drop() {

    if(data != NULL){
        free(data);
    }
    if(pkt != NULL){
        av_packet_free(&pkt);
    }
}