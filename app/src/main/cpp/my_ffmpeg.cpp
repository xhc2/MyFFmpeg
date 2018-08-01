//
// Created by Administrator on 2018/4/21/021.
//

//adb logcat | ndk-stack -sym obj/local/armeabi 查看底层错误栈
//NDK 使用 addr2line 调试工具
#include <jni.h>
#include <my_log.h>
#include <Mp4Player.h>
#include <gpu_video_sl_audio.h>
#include <CallJava.h>
#include <PublishStream.h>


/**
 * 返回 >=0 表示成功
 * 其他表示失败
 * @param env
 * @param clazz
 * @return
 */
// using namespace soundtouch;



//JNIEXPORT jstring JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative(JNIEnv *env, jclass clazz) {
//
//    //如果是c++就是这种方式，如果是c需要（*env）->
//    return env->NewStringUTF("ffmpeg hello world ");
//}
//
//JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decodeMp4ToYuv
//        (JNIEnv *env, jclass clazz, jstring path, jobject surface) {
//    int result = 0;
//    const char *input_str = env->GetStringUTFChars(path, NULL);
//    LOGE("input path %s ", input_str);
//    decode(input_str, env, surface);
//    env->ReleaseStringUTFChars(path, input_str);
//    return result;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLTest(JNIEnv *env, jclass clazz, jstring path) {
//    const char *input_str = env->GetStringUTFChars(path, NULL);
//    play_audio(input_str);
//    env->ReleaseStringUTFChars(path, input_str);
//    return 1;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLPauseOrPlay(JNIEnv *env, jclass clazz,
//                                                             jboolean flag) {
//    pause(flag);
//    return 1;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_openSLDestroy(JNIEnv *env, jclass clazz) {
//    return openslDestroy();
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_decodeMp4ToYuvShowShader(JNIEnv *env, jclass clazz,
//                                                                    jstring path, jobject surface) {
//    const char *input_str = env->GetStringUTFChars(path, NULL);
//    openAndShowUseShader(input_str, env, surface);
//    env->ReleaseStringUTFChars(path, input_str);
//    return 1;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_videoAudioDecodeShow(JNIEnv *env, jclass clazz,
//                                                                jstring path, jobject surface) {
//    const char *input_str = env->GetStringUTFChars(path, NULL);
//    videoAudioOpen(env, surface, input_str);
//    env->ReleaseStringUTFChars(path, input_str);
//    return 1;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_testMyShow(JNIEnv *env, jclass clazz) {
////    videoAudioOpen(env , surface , input_str);
//    testPlay();
//    return 1;
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThread(JNIEnv *env, jclass clazz) {
//    return initThread();
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThreadFree(JNIEnv *env, jclass clazz) {
//    return threadFree();
//}
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_testNativeThreadRun(JNIEnv *env, jclass clazz) {
//    return threadRun();
//}
//
//
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_testCPlusPlusThread(JNIEnv *env, jclass clazz) {
//    LOGE("TEST C++ THREAD");
//    initMyCPlusPlusThread();
//    return 1 ;
//}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuAudioOpensl(JNIEnv *env, jclass type,
//                                                                   jstring path_,
//                                                                   jobject glSurfaceView) {
//    const char *path = env->GetStringUTFChars(path_, 0);
//
//    open_gpu(env , path , glSurfaceView);
//
//    env->ReleaseStringUTFChars(path_, path);
//    return 1;
//}
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuDestroy(JNIEnv *env, jclass type) {
//
//    // TODO
//    return destroy_gpu();
//
//}
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuJustPause(JNIEnv *env, jclass type) {
//
//    // TODO
//    return justPause_gpu();
//
//}
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuPlayOrPause(JNIEnv *env, jclass type){
//
//    return playOrPause_gpu();
//}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_showVideoGpuSeek(JNIEnv *env, jclass type,
//                                                            jdouble seek) {
//
//    return seekPos(seek);
//}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_getPlayPosition(JNIEnv *env, jclass type) {
//
//    // TODO
//    return getPlayPosition();
//}
//
//
//extern "C"
//JNIEXPORT jstring JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_getVersionString(JNIEnv *env, jclass type) {
//
//   const char *verStr;
//   verStr = "soundtouch 已移除";
//   return env->NewStringUTF(verStr);
//}
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_changeSpeedGpu(JNIEnv *env, jclass type, jdouble speed) {
//
//    return changeSpeed(speed);
//}
//
//
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_soundtouchStart(JNIEnv *env, jclass type) {
//
//    // TODO
//    return 1;
//}
//
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_soundtouchSpeed(JNIEnv *env, jclass type,
//                                                           jdouble speed) {
//
//    return 1;
//}
//
////extern "C"
////JNIEXPORT jint JNICALL
////Java_module_video_jnc_myffmpeg_FFmpegUtils_initsoundtouch(JNIEnv *env, jclass type , jstring path_) {
////    const char *path = env->GetStringUTFChars(path_, 0);
////    mySoundTouchInstance = new mySoundTouch();
////    mySoundTouchInstance->init(path);
////    env->ReleaseStringUTFChars(path_, path);
////    return 1;
////}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_initsoundtouch(JNIEnv *env, jclass type, jstring path_) {
//    const char *path = env->GetStringUTFChars(path_, 0);
//
//    mySoundTouchInstance = new mySoundTouch();
//    LOGE("input paht %s" , path);
//    mySoundTouchInstance->init(path);
//
//    env->ReleaseStringUTFChars(path_, path);
//    return 1;
//}
Mp4Player *mp4Player = NULL ;
PublishStream *ps = NULL;
CallJava *cj = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initMp4Play(JNIEnv *env, jclass type, jstring path_,
                                                       jobject glSurfaceView) {

    if(mp4Player == NULL){
        if(cj == NULL){
            cj = new CallJava(env , type);
        }
        const char *path = env->GetStringUTFChars(path_, 0);
        ANativeWindow *win = ANativeWindow_fromSurface(env, glSurfaceView);
        mp4Player = new Mp4Player(path ,win  , cj );
        env->ReleaseStringUTFChars(path_, path);
    }
    return 1;

}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyMp4Play(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        delete mp4Player;
        mp4Player = NULL;
    }
    if(cj != NULL){
        delete cj;
        cj = NULL;
    }
    return 1;

}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_mp4Pause(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        mp4Player->pauseVA();
        return 1;
    }

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_mp4Play(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        mp4Player->playVA();
        return 1;
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_changeSpeed(JNIEnv *env, jclass type, jfloat speed) {
    // TODO
    if(mp4Player != NULL){
        mp4Player->changeSpeed(speed);
        return 1;
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getProgress(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        return mp4Player->getProgress();
    }
    return 0;
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getDuration(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        return mp4Player->getDuration();
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_seekStart(JNIEnv *env, jclass type) {
    if(mp4Player != NULL){
        mp4Player->seekStart();
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_seek(JNIEnv *env, jclass type  , jfloat progress) {
    if(mp4Player != NULL){
        mp4Player->seek(progress);
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpInit(JNIEnv *env, jclass type  , jstring path_ , jstring inpath_) {

    const char *path = env->GetStringUTFChars(path_, 0);
    const char *inPath = env->GetStringUTFChars(inpath_, 0);
    if(ps == NULL){
        if(cj == NULL){
            cj = new CallJava(env , type);
        }
        ps = new PublishStream(path , inPath , cj);
    }
    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(inpath_, inPath);

    return 1;

}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpClose(JNIEnv *env, jclass type) {

    // TODO
    if(ps != NULL){
        delete ps;
        ps = NULL;
    }
    if(cj != NULL){
        delete cj;
        cj = NULL;
    }
    return 1;
}