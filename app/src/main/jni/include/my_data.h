//
// Created by Administrator on 2018/5/2/002.
//
#ifndef MYFFMPEG_MY_DATA_H
#define MYFFMPEG_MY_DATA_H

class MyData{

public :
    char* data;
    //audio true ， video false
    bool isAudio = false;
    int size = 0;
    int pts;
};

#endif //MYFFMPEG_MY_DATA_H
