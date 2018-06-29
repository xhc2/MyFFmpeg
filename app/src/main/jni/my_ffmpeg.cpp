//
// Created by Administrator on 2018/4/21/021.
//

//adb logcat | ndk-stack -sym obj/local/armeabi 查看底层错误栈
#include "my_ffmpeg.h"
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <my_log.h>
#include <decode_show_gl.h>
#include <my_c_plus_plus_thread.h>
#include <gpu_video_sl_audio.h>
#include "decode_encode_test.h"
#include "my_open_sl_test.h"
#include "video_audio_decode_show.h"
#include "my_thread.h"
#include "SoundTouch.h"
#include "WavFile.h"
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
// using namespace soundtouch;

using namespace soundtouch;

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
    openAndShowUseShader(input_str, env, surface);
    env->ReleaseStringUTFChars(path, input_str);
    return 1;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_videoAudioDecodeShow(JNIEnv *env, jclass clazz,
                                                                jstring path, jobject surface) {
    const char *input_str = env->GetStringUTFChars(path, NULL);
    videoAudioOpen(env, surface, input_str);
    env->ReleaseStringUTFChars(path, input_str);
    return 1;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_testMyShow(JNIEnv *env, jclass clazz) {
//    videoAudioOpen(env , surface , input_str);
    testPlay();
    return 1;
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThread(JNIEnv *env, jclass clazz) {
    return initThread();
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThreadFree(JNIEnv *env, jclass clazz) {
    return threadFree();
}

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThreadRun(JNIEnv *env, jclass clazz) {
    return threadRun();
}


JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_testCPlusPlusThread(JNIEnv *env, jclass clazz) {
    LOGE("TEST C++ THREAD");
    initMyCPlusPlusThread();
    return 1 ;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuAudioOpensl(JNIEnv *env, jclass type,
                                                                   jstring path_,
                                                                   jobject glSurfaceView) {
    const char *path = env->GetStringUTFChars(path_, 0);

    open_gpu(env , path , glSurfaceView);

    env->ReleaseStringUTFChars(path_, path);
    return 1;
}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuDestroy(JNIEnv *env, jclass type) {

    // TODO
    return destroy_gpu();

}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuJustPause(JNIEnv *env, jclass type) {

    // TODO
    return justPause_gpu();

}
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuPlayOrPause(JNIEnv *env, jclass type){

    return playOrPause_gpu();
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuSeek(JNIEnv *env, jclass type,
                                                            jdouble seek) {

    return seekPos(seek);
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getPlayPosition(JNIEnv *env, jclass type) {

    // TODO
    return getPlayPosition();
}


extern "C"
JNIEXPORT jstring JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getVersionString(JNIEnv *env, jclass type) {

   const char *verStr;
   verStr = SoundTouch::getVersionString();
   return env->NewStringUTF(verStr);
}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_changeSpeedGpu(JNIEnv *env, jclass type, jdouble speed) {

    return changeSpeed(speed);
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_soundtouchStart(JNIEnv *env, jclass type) {

    // TODO
    return 1;
}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_soundtouchSpeed(JNIEnv *env, jclass type,
                                                           jdouble speed) {

    return 1;
}