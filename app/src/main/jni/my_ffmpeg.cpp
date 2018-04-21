//
// Created by Administrator on 2018/4/21/021.
//

#include "my_ffmpeg.h"
#include <jni.h>
#include <string.h>
#include "My_LOG.h"
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include "libavcodec/avcodec.h"


JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative(JNIEnv *env ,jclass clazz){

    //如果是c++就是这种方式，如果是c需要（*env）->
    return env->NewStringUTF("ffmpeg hello world ");
}

