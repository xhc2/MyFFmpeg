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

  
  
  
  
  
  
  
  