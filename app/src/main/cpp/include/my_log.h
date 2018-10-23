

#ifndef MYANDROIDRTMP_MYLOG_H
#define MYANDROIDRTMP_MYLOG_H


#include <jni.h>
#include <Android/log.h>
#include <string.h>
#include <stdio.h>

#define LOG "xhc_jni"
#define LOGF_TAG "ffmpeg"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
#define LOGFFMPEG(...)  __android_log_print(ANDROID_LOG_INFO,LOGF_TAG,__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG,__VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG,__VA_ARGS__)
#define LOGE2(...)  __android_log_print(ANDROID_LOG_ERROR,"xhc_jni2",__VA_ARGS__)
const static int RESULT_SUCCESS = 1;
const static int RESULT_FAILD = -1;
//目前不支持的像素格式
const static int RESULT_WRONG_PIX = -100;

#endif //MYANDROIDRTMP_MYLOG_H