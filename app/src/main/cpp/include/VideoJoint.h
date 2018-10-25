//
// Created by Administrator on 2018/10/25/025.
//

#ifndef MYFFMPEG_VIDEOJOINT_H
#define MYFFMPEG_VIDEOJOINT_H

#include <string.h>
#include <jni.h>
#include <my_log.h>
#include <vector>
#include "EditParent.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
}

using namespace std;

class VideoJoint  {

private :
    char *outPath;
    vector <char *> inputPaths;

    int video_index;
    int audio_index;

    AVCodec *videoCodecD;
    AVCodec *videoCodecE;

    AVCodec *audioCodecE;
    AVCodec *audioCodecD;

    AVCodecContext *aCtxD;
    AVCodecContext *aCtxE;

    AVCodecContext *vCtxD;
    AVCodecContext *vCtxE;

    AVFormatContext *afc_input;
    AVFormatContext *afc_output;
    AVOutputFormat *afot;

    int addVideoOutputStream(int width , int height);
    int addAudioOutputStream();

    AVStream *audioOutStream ;
    AVStream *videoOutStream ;

    int outWidth ;
    int outHeight;

public :
    VideoJoint(vector <char *> inputPath ,   const char *output , int outWidth , int outHeight);

    void startJoint();

    int initInput(char *path);

    void destroyInput();

    void destroyOutput();

    int initOutput(char *path);

    ~VideoJoint();
};

#endif //MYFFMPEG_VIDEOJOINT_H
