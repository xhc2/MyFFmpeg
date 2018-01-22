#include <module_video_jnc_myffmpeg_FFmpegUtils.h>
#include <string.h>
#include "My_LOG.h"
#include <time.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include "libavutil/log.h"
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <myfiler.h>
#include <mydecode.h>
#include <mystream.h>
#include <filter_video.h>
#include <swscale.h>
#include <my_muxer.h>
#include <my_ffmpeg.h>
#include <my_camera_muxer.h>
#include <my_audio_record.h>

JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative
        (JNIEnv *env, jclass clazz) {

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}

/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    stringJni
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringJni
        (JNIEnv *env, jclass clazz) {
    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}

/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    decode
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 * 解码
 */
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decode
        (JNIEnv *env, jclass clazz, jstring input_jstr, jstring output_jstr) {

    const char *input_str = NULL;
    const char *output_str = NULL;
    input_str = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    output_str = (*env)->GetStringUTFChars(env, output_jstr, NULL);

    decode(input_str, output_str);

    (*env)->ReleaseStringUTFChars(env, input_jstr, input_str);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_str);
    return 0;
}

/*
* Class:     module_video_jnc_myffmpeg_FFmpegUtils
* Method:    stream
* Signature: (Ljava/lang/String;Ljava/lang/String;)I
* 推流
*/
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stream
        (JNIEnv *env, jclass clazz, jstring input_jstr, jstring output_jstr) {
    const char *input_str = NULL;
    const char *output_str = NULL;

    input_str = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    output_str = (*env)->GetStringUTFChars(env, output_jstr, NULL);
    stream(input_str, output_str);
    (*env)->ReleaseStringUTFChars(env, input_jstr, input_str);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_str);
    return 0;
}

/*
* Class:     module_video_jnc_myffmpeg_FFmpegUtils
* Method:    encode
* Signature: (Ljava/lang/String;)I
* 将MP4格式的数据转码成flv（编码格式也改变下h264，改变成随便一个格式。）
 * 转码是先h264 -》 yuv -》 其他格式
*/
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encode
        (JNIEnv *env, jclass clazz, jstring jstr_inputPath, jstring jstr_outPath) {

    const char *input_str = (*env)->GetStringUTFChars(env, jstr_inputPath, NULL);
    const char *output_str = (*env)->GetStringUTFChars(env, jstr_outPath, NULL);
    encode_h264(input_str, output_str);
    (*env)->ReleaseStringUTFChars(env, jstr_inputPath, input_str);
    (*env)->ReleaseStringUTFChars(env, jstr_outPath, output_str);
    return 0;
}


JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encodeYuv
        (JNIEnv *env, jclass clazz, jstring jstr_input, jstring jstr_output) {
    av_register_all();
    const char *input_str = (*env)->GetStringUTFChars(env, jstr_input, NULL);
    const char *output_str = (*env)->GetStringUTFChars(env, jstr_output, NULL);
    encodeYuv(input_str, output_str);
    (*env)->ReleaseStringUTFChars(env, jstr_input, input_str);
    (*env)->ReleaseStringUTFChars(env, jstr_output, output_str);

}

/*
* Class:     module_video_jnc_myffmpeg_FFmpegUtils
* Method:    addfilter
* Signature: (Ljava/lang/String;Ljava/lang/String;)I
 * 打水印并输出yuv文件
 *
 * http://ffmpeg.org/doxygen/3.2/filtering_video_8c-example.html#a48
*/
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_addfilter
        (JNIEnv *env, jclass clazz, jstring inputStr, jstring outputStr) {
    const char *input_str = (*env)->GetStringUTFChars(env, inputStr, NULL);
    const char *output_str = (*env)->GetStringUTFChars(env, outputStr, NULL);
    LOGE("addfilter input str %s , outputstr %s", input_str, output_str);
//    main2(input_str, output_str);
    filter_video( input_str, output_str);
    (*env)->ReleaseStringUTFChars(env, inputStr, input_str);
    (*env)->ReleaseStringUTFChars(env, outputStr, output_str);
    return 0;
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_swscale
        (JNIEnv *env, jclass clazz, jstring inputStr, jstring outputStr) {
    const char *input_str = (*env)->GetStringUTFChars(env, inputStr, NULL);
    const char *output_str = (*env)->GetStringUTFChars(env, outputStr, NULL);
    LOGE("addfilter input str %s , outputstr %s", input_str, output_str);
    swscale( input_str, output_str);
    (*env)->ReleaseStringUTFChars(env, inputStr, input_str);
    (*env)->ReleaseStringUTFChars(env, outputStr, output_str);
    return 0;
}


JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_muxer
        (JNIEnv *env, jclass clazz, jstring inputStr, jstring outputVStr, jstring outputAStr) {
    const char *input_str = (*env)->GetStringUTFChars(env, inputStr, NULL);
    const char *output_v_str = (*env)->GetStringUTFChars(env, outputVStr, NULL);
    const char *output_a_str = (*env)->GetStringUTFChars(env, outputAStr, NULL);
    LOGE("muxer input str %s , outputvstr %s , outputastr %s", input_str, output_v_str , output_a_str);
    muxer( input_str, output_v_str  , output_a_str);
    (*env)->ReleaseStringUTFChars(env, inputStr, input_str);
    (*env)->ReleaseStringUTFChars(env, outputVStr, output_v_str);
    (*env)->ReleaseStringUTFChars(env, outputAStr, output_a_str);
    return 0;
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_demuxer
        (JNIEnv *env, jclass clazz, jstring inputStr, jstring outputVStr, jstring outputAStr) {
    const char *input_str = (*env)->GetStringUTFChars(env, inputStr, NULL);
    const char *output_v_str = (*env)->GetStringUTFChars(env, outputVStr, NULL);
    const char *output_a_str = (*env)->GetStringUTFChars(env, outputAStr, NULL);
    LOGE("demuxer input_str %s ,  outputvstr %s , outputastr %s", input_str, output_v_str , output_a_str);
    demuxer( input_str, output_v_str  , output_a_str);
    (*env)->ReleaseStringUTFChars(env, inputStr, input_str);
    (*env)->ReleaseStringUTFChars(env, outputVStr, output_v_str);
    (*env)->ReleaseStringUTFChars(env, outputAStr, output_a_str);
    return 0;
}

//初始化
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_myInit(JNIEnv *env, jclass clazz , jstring outputVStr , int width , int height){
    int result = 0;
    const char *output_v_str = (*env)->GetStringUTFChars(env, outputVStr, NULL);
    result = init(output_v_str , width , height);
//    LOGE(" RESULT %s " ,output_v_str );
    (*env)->ReleaseStringUTFChars(env, outputVStr, output_v_str);
    return result;
}


//关闭
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_closeMyFFmpeg(JNIEnv *env, jclass clazz){
    return close();
}

//将camera的数据的编码
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encodeCamera(JNIEnv *env, jclass clazz ,jbyteArray yuvArray){
    //第三个参数确定返回的是指向的副本还是固定对象
    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , yuvArray , JNI_FALSE);
    encodeCamera(navtiveYuv);
    //JNI_ABORT 释放原生数组但不用将内容复制回来
    (*env)->ReleaseByteArrayElements(env , yuvArray , navtiveYuv , JNI_ABORT);
    return 0;
}

//将nv21的格式的数据转成yv12，就是将package格式的转成平面格式的，好让ffmpeg编码
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_nv21ToYv12(JNIEnv *env, jclass clazz , jbyteArray yuvArray){

    jboolean flag = JNI_FALSE;
    int length = (*env)->GetArrayLength(env,yuvArray); //获取长度
    LOGE("YUV LENGTH %d " , length);
    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , yuvArray , &flag);
    nv21ToYv12(navtiveYuv);
    //0的意思是将内容复制过来，并释放原生数组
    (*env)->ReleaseByteArrayElements(env , yuvArray , navtiveYuv , 0);

}




JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encodePcm(JNIEnv *env, jclass clazz , jbyteArray pcmArray , jint size){

    jboolean copy = JNI_FALSE;

    jbyte *navtivePcm = (*env)->GetByteArrayElements(env , pcmArray , &copy);

    encodePcm(navtivePcm , size);

    (*env)->ReleaseByteArrayElements(env , pcmArray , navtivePcm , 0);

}


//public static native int initMyCameraMuxer(String outputPath , int width , int height , int aSize);
//
//                     public static native int encodeMyMuxerCamera(byte[] bytes);
//
//                     public static native int encodeMyMuxerAudio(byte[] bytes);
//
//                     public static native int closeMyMuxer();

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_initMyCameraMuxer(JNIEnv *env, jclass clazz ,
                                                                                    jstring joutputPath , jint width , jint height , jint aSize){
    int ret = 0;
    const char *output_path= (*env)->GetStringUTFChars(env, joutputPath, NULL);
    ret = init_camera_muxer(output_path , width , height , aSize);
//    LOGE(" RESULT %s " ,output_v_str );
    (*env)->ReleaseStringUTFChars(env, joutputPath, output_path);

    return ret;
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encodeMyMuxerCamera(JNIEnv *env, jclass clazz ,jbyteArray yuvArray){

    int ret = 0 ;
    jboolean copy = JNI_FALSE;

    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , yuvArray , &copy);
    ret =  encodeCamera_muxer(navtiveYuv);
    //0的意思是将内容复制过来，并释放原生数组
    (*env)->ReleaseByteArrayElements(env , yuvArray , navtiveYuv , JNI_ABORT);

    return ret;
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encodeMyMuxerAudio(JNIEnv *env, jclass clazz ,jbyteArray pcmArray){
    int ret = 0 ;
    jboolean copy = JNI_FALSE;

    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , pcmArray , &copy);
    ret =  encodeAudio_muxer(navtiveYuv);
    //0的意思是将内容复制过来，并释放原生数组
    (*env)->ReleaseByteArrayElements(env , pcmArray , navtiveYuv , JNI_ABORT);
    return ret;
}
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_closeMyMuxer(JNIEnv *env, jclass clazz ){


    return close_muxer();
}


JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_init_audio_(JNIEnv *env, jclass clazz ,jstring joutputPath ,  jint aSize){
    int ret = 0;
    const char *output_path= (*env)->GetStringUTFChars(env, joutputPath, NULL);
    ret = init_audio(output_path , aSize);
    (*env)->ReleaseStringUTFChars(env, joutputPath, output_path);

    return ret ;
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_close_audio_(JNIEnv *env, jclass clazz  ){


    return close_audio();
}

JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encode_audio_(JNIEnv *env, jclass clazz ,jbyteArray pcmArray){

    int ret = 0 ;
    jboolean copy = JNI_FALSE;

    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , pcmArray , &copy);
    ret =  encode_audio_(navtiveYuv);
    //0的意思是将内容复制过来，并释放原生数组
    (*env)->ReleaseByteArrayElements(env , pcmArray , navtiveYuv , JNI_ABORT);

    return ret;
}



/**
 *GetByteArrayElements 中的第三个参数 和 ReleaseByteArrayElements 第三个参数可以配合使用。
 * JNI_FALSE 指向副本
 * JNI_TRUE 指向java的原始堆的固定对象
 *
 * ReleaseByteArrayElements
 * JNI_ABORT 释放原生数组但不用将内容复制过来
 * 0 将内容复制回来并释放数组
 * JNI_COMMIT 将内容复制过来，但是不释放原生数组
 *
 *
 * 当JNI_ABORT时不管JNI_FALSE ， JNI_TRUE原始的数组都不会改变。
 * 当JNI_ABORT或者0时原生数组好像都并没有释放，但是看内存也没有内存溢出。
 *
 */
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_testArray(JNIEnv *env, jclass clazz , jbyteArray yuvArray){

    jboolean copy = JNI_FALSE;

    jbyte *navtiveYuv = (*env)->GetByteArrayElements(env , yuvArray , &copy);

    navtiveYuv[0] = 1;
    navtiveYuv[1] = 1;
    navtiveYuv[2] = 1;
    //0的意思是将内容复制过来，并释放原生数组
    (*env)->ReleaseByteArrayElements(env , yuvArray , navtiveYuv , 0);

    LOGE(" jni %d", navtiveYuv[0] );
    LOGE(" jni %d", navtiveYuv[1] );
    LOGE(" jni %d", navtiveYuv[2] );
}
