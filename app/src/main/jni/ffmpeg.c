//
//

#include <module_video_jnc_myffmpeg_FFmpegUtils.h>
#include <string.h>
#include "libavcodec/avcodec.h"
#include "My_LOG.h"

//Error:(5, 51) module_video_jnc_myffmpeg_FFmpegUtils.h: No such file or directory
JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative
        (JNIEnv *env, jclass clazz){

    char info[10000] = { 0 };
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}

/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    stringJni
 * Signature: ()Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringJni
        (JNIEnv *env, jclass clazz){
    char info[10000] = { 0 };
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}


/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    decode
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT void JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decode
  (JNIEnv *env, jclass clazz, jstring fileInput, jstring outputStr){
	  
	  
	 
	  const char *inputPath =(*env)->GetStringUTFChars(env,fileInput,NULL);
	  const char *outputPath = (*env)->GetStringUTFChars(env,outputStr,NULL);
	  
	   LOGE(" fileinputPath %s , fileoutput %s ",inputPath , outputPath);
	   
	  (*env)->ReleaseStringUTFChars(env ,inputPath, fileInput);
	  (*env)->ReleaseStringUTFChars(env ,outputStr, outputStr);
	  
  }
  
  
  
  
  
  