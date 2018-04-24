//
// Created by Administrator on 2018/4/21/021.
//

#include "my_ffmpeg.h"
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <my_log.h>
#include <decode_show_gl.h>
#include "decode_encode_test.h"
#include "my_open_sl_test.h"

//void testPointer(char* test){
//    test = (char *)malloc(sizeof(char) * 6);
//    LOGE("TEST HANSHU %d " , test);
//}

/**
 * 返回 >=0 表示成功
 * 其他表示失败
 * @param env
 * @param clazz
 * @return
 */
JNIEXPORT jstring JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative(JNIEnv *env, jclass clazz) {

    //如果是c++就是这种方式，如果是c需要（*env）->
    return env->NewStringUTF("ffmpeg hello world ");
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decodeMp4ToYuv
        (JNIEnv *env, jclass clazz, jstring path, jobject surface) {
    int result = 0;
    const char *input_str = env->GetStringUTFChars(path, NULL);
    LOGE("input path %s ", input_str);
    decode(input_str, env, surface);
    env->ReleaseStringUTFChars(path, input_str);
    return result;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLTest(JNIEnv *env, jclass clazz, jstring path) {
    const char *input_str = env->GetStringUTFChars(path, NULL);
    LOGE("input path %s ", input_str);
    play_audio(input_str);
    env->ReleaseStringUTFChars(path, input_str);
    return 1;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLPauseOrPlay(JNIEnv *env, jclass clazz,
                                                             jboolean flag) {
    pause(flag);
    return 1;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLDestroy(JNIEnv *env, jclass clazz) {
    return openslDestroy();
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_decodeMp4ToYuvShowShader(JNIEnv *env, jclass clazz,
                                                                    jstring path, jobject surface) {
    const char *input_str = env->GetStringUTFChars(path, NULL);
    openAndShowUseShader(input_str ,  surface);
    env->ReleaseStringUTFChars(path, input_str);
    return  1;
}