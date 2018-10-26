//
// Created by Administrator on 2018/10/25/025.
//

#ifndef MYFFMPEG_VIDEOJOINT_H
#define MYFFMPEG_VIDEOJOINT_H

#include <string.h>
#include <jni.h>
#include <my_log.h>
#include <vector>
#include <queue>
#include "EditParent.h"
#include "MyThread.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
}

using namespace std;

class VideoJoint : public MyThread {

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

    AVStream *audioOutStream ;
    AVStream *videoOutStream ;

    int outWidth ;
    int outHeight;

    SwsContext *sws;

    AVFrame *outVFrame ;


    queue<AVPacket *> audioQue;
    queue<AVPacket *> VideoQue;

    int initSwsContext(int inWidth , int inHeight , int inpixFmt);
    void destroySwsContext();
    void initValue();
    int addVideoOutputStream(int width , int height);
    int addAudioOutputStream();
    AVPacket *encodeFrame(AVFrame *frame , AVCodecContext *encode);
    AVFrame *deocdePacket(AVPacket *packet , AVCodecContext *decode );
    void startDecode();
    void addQueue(AVPacket *pkt);

public :
    VideoJoint(vector <char *> inputPath ,   const char *output , int outWidth , int outHeight);

    void startJoint();

    int initInput(char *path);

    void destroyInput();

    void destroyOutput();

    int initOutput(char *path);

    ~VideoJoint();

    virtual void run();
};

#endif //MYFFMPEG_VIDEOJOINT_H
