//
// Created by dugang on 2018/7/4.
//
#include <my_log.h>
#include "my_data.h"

MyData::MyData(){
    data = NULL;
    pkt = NULL;
    for(int i = 0 ;i < 8 ; ++ i){
        datas[i] = NULL;
    }
    isAudio = false;
};

MyData::~MyData(){
    drop();
};


void MyData::drop() {

    if(data != NULL){
        free(data);
    }
    if(pkt != NULL){
        av_packet_free(&pkt);
    }
    for(int i = 0 ;i < 8 ; ++ i){
        if( datas[i] != NULL){
            free(datas[i]);
        }
    }
}