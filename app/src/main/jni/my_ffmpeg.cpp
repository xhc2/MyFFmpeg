//
// Created by Administrator on 2018/4/21/021.
//

#include "my_ffmpeg.h"
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <my_log.h>
#include "decode_encode_test.h"

/**
 * 返回 >=0 表示成功
 * 其他表示失败
 * @param env
 * @param clazz
 * @return
 */
JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative(JNIEnv *env ,jclass clazz){

    //如果是c++就是这种方式，如果是c需要（*env）->
    return env->NewStringUTF("ffmpeg hello world ");
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decodeMp4ToYuvPcm
        (JNIEnv  *env ,jclass clazz , jstring path ,  jobject surface ){
    int result = 0;
   const char *input_str = env->GetStringUTFChars(path, NULL);
    LOGE("input path %s ", input_str);
    decode(input_str , env , surface);
    env->ReleaseStringUTFChars( path, input_str);
    return result;
}