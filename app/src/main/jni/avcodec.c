#include <jni.h>

JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpCameraStream(JNIEnv *env, jclass type,
                                                            jbyteArray bytes_) {
    jbyte *bytes = (*env)->GetByteArrayElements(env, bytes_, NULL);

    // TODO

    (*env)->ReleaseByteArrayElements(env, bytes_, bytes, 0);
}