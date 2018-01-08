//
// Created by dugang on 2018/1/5.
//
#include <jni.h>
#ifndef MYFFMPEG_MY_FFMPEG_H
#define MYFFMPEG_MY_FFMPEG_H
int init(const char *ouputPath , int width , int height);
int close();
int encodeCamera(jbyte *navtiveYuv);
#endif //MYFFMPEG_MY_FFMPEG_H


