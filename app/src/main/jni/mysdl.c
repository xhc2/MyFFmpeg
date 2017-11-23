//
// Created by dugang on 2017/11/23.
//
#include <module_video_jnc_myffmpeg_SDLUtils.h>
#include <string.h>
#include "My_LOG.h"
#include <time.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_log.h"
#include "SDL_main.h"

/*
 * Class:     module_video_jnc_myffmpeg_SDLUtils
 * Method:    initSDLTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_module_video_jnc_myffmpeg_SDLUtils_initSDLTest
        (JNIEnv *env, jclass clazz){

    LOGE(" SDL TEST START");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1) {
        LOGE("SDL_Init failed %s", SDL_GetError());
    }

    LOGE(" SDL TEST START SUCCESS ");
}