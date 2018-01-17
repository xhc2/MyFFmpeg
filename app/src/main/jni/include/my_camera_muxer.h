//
// Created by dugang on 2018/1/15.
//

#include <jni.h>
#ifndef MYFFMPEG_MY_CAMERA_MUXER_H
#define MYFFMPEG_MY_CAMERA_MUXER_H
int init_camera_muxer(const char *outputPath , int width , int height , int aSize);
int encodeCamera_muxer(jbyte *nativeYuv);
int encodeAudio_muxer(jbyte *nativePcm);
int close_muxer();

int initMuxerVideo();
int initMuxerAudio();

int encodePcm_(jbyte *nativePcm);
int encodeYuv_(jbyte *nativeYuv);
#endif //MYFFMPEG_MY_CAMERA_MUXER_H
//Error:(18, 5) error: conflicting types for 'encodePcm'