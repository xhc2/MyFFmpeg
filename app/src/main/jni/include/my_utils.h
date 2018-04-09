//
// Created by Administrator on 2018/2/3/003.
//
#include <jni.h>
#ifndef MYFFMPEG_MY_UTILS_H
#define MYFFMPEG_MY_UTILS_H
void utils_nv21ToYv12(jbyte *navtiveYuv ,int y_size);
void myThreadStart(JNIEnv *env, jclass clazz);
void myThreadStop(JNIEnv *env, jclass clazz);
void myThreadInit(JNIEnv *env, jclass clazz);
#endif //MYFFMPEG_MY_UTILS_H
