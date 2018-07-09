//
// Created by dugang on 2018/4/25.
//

#ifndef MYFFMPEG_VIDEO_AUDIO_DECODE_SHOW_H
#define MYFFMPEG_VIDEO_AUDIO_DECODE_SHOW_H

#include <jni.h>

int videoAudioOpen(JNIEnv *env , jobject surface , const char* path);
void testPlay();
void ThreadSleep(int mis);

#endif //MYFFMPEG_VIDEO_AUDIO_DECODE_SHOW_H
