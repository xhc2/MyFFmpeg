//
// Created by Administrator on 2018/1/20/020.
//
#include <jni.h>
#ifndef MYFFMPEG_MY_AUDIO_RECORD_H
#define MYFFMPEG_MY_AUDIO_RECORD_H

int init_audio(const char*output_path , int a_size);
int close_audio();
int encode_audio_(jbyte *nativepcm);
int initAudio_record();
#endif //MYFFMPEG_MY_AUDIO_RECORD_H
