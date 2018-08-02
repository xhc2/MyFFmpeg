
#include <malloc.h>
#include <my_log.h>
#include <CallJava.h>
#include "CameraStream.h"

CameraStream::CameraStream(const char * url , int width , int height , CallJava *cj ){
    this->url = url;
    this->width = width;
    this->height = height;
    this->size = width * height * 1.5f ;
    LOGE("SIZE %d " , size);
    this->cj = cj;
    yuv = NULL;
    yuv = (char *) malloc(size );
    if(yuv == NULL){
        cj->callStr("YUV ALLOC FAILD2 !");
        return ;
    }
    file = fopen("sdcard/FFmpeg/yuv.yuv" , "wb+");
    LOGE(" YUV ALLOC success !");
}

CameraStream::~CameraStream(){

}

void CameraStream:: pushStream(jbyte *yuv){
    memcpy(this->yuv , yuv ,  size);
    fwrite(this->yuv  ,size , 1 ,  file );
}