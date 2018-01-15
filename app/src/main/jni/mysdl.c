#include <string.h>
#include "My_LOG.h"
#include <time.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_log.h"
#include "SDL_main.h"
#include <jni.h>
#include <android/log.h>
#include <src/SDL_internal.h>
#include <SDL_quit.h>
#include <SDL.h>

//int main(int argc, char *argv[]) {
//
//    struct SDL_Window *window = NULL;
//    struct SDL_Renderer *render = NULL;
//    struct SDL_Surface *bmp = NULL;
//    struct SDL_Texture *texture = NULL;
//
//    char *filepath = "/storage/emulated/0/test.bmp";
//
//    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1) {
//        LOGE("SDL_Init failed %s", SDL_GetError());
//    }
//
//    window = SDL_CreateWindow("SDL HelloWorld!", 100, 100, 640, 480,
//            SDL_WINDOW_SHOWN);
//    if (window == NULL) {
//        LOGE("SDL_CreateWindow failed  %s", SDL_GetError());
//    }
//
//    render = SDL_CreateRenderer(window, -1,
//            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
//    if (render == NULL) {
//        LOGE("SDL_CreateRenderer failed  %s", SDL_GetError());
//    }
//
//    bmp = SDL_LoadBMP(filepath);
//    if (bmp == NULL) {
//        LOGE("SDL_LoadBMP failed: %s", SDL_GetError());
//    }
//
//    texture = SDL_CreateTextureFromSurface(render, bmp);
//    SDL_FreeSurface(bmp);
//
//    SDL_RenderClear(render);
//    SDL_RenderCopy(render, texture, NULL, NULL);
//    SDL_RenderPresent(render);
//
//    SDL_Delay(10000);
//
//    SDL_DestroyTexture(texture);
//    SDL_DestroyRenderer(render);
//    SDL_DestroyWindow(window);
//
//    //Quit SDL
//    SDL_Quit();
//    return 0;
//}

/*
 * Class:     module_video_jnc_myffmpeg_SDLUtils
 * Method:    initSDLTest
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_module_video_jnc_myffmpeg_SDLUtils_initSDLTest
        (JNIEnv *env, jclass clazz){

    LOGE(" SDL TEST START");

//    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1) {
//        LOGE("SDL_Init failed %s", SDL_GetError());
//    }

    LOGE(" SDL TEST START SUCCESS ");
}