//
// Created by Administrator on 2018/5/2/002.
//
#ifndef MYFFMPEG_MY_DATA_H
#define MYFFMPEG_MY_DATA_H

extern "C"{
#include <libavcodec/avcodec.h>
};


class MyData{

public :
    char* data;
    uint8_t *datas[8];
    int linesize[8];
    AVPacket *pkt;
    //audio true ， video false
    bool isAudio ;
    int size ;
    int vWidth ;
    int vHeight ;
    int64_t pts;
    void drop();
    MyData();
    ~MyData();
};

#endif //MYFFMPEG_MY_DATA_H
