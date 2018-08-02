//
// Created by dugang on 2018/8/2.
//

#ifndef MYFFMPEG_CAMERASTREAM_H
#define MYFFMPEG_CAMERASTREAM_H

#include <stdio.h>

class CameraStream{

private :

    const char *url;
    char *yuv;
    int width , height;
    int size ;
    //用来测试使用。
    FILE *file;
    CallJava *cj ;
public :
    CameraStream(const char * url , int width , int height , CallJava *cj);
    ~CameraStream();
    void pushStream(jbyte *yuv);

};

#endif //MYFFMPEG_CAMERASTREAM_H
