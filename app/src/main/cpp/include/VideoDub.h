//
// Created by Administrator on 2018/12/13/013.
//

#ifndef MYFFMPEG_VIDEODUB_H
#define MYFFMPEG_VIDEODUB_H

#include <android/native_window.h>
#include "EditParent.h"
#include "MyThread.h"
#include "YuvPlayer.h"

class VideoDub : public EditParent ,public MyThread , public Notify {
private :
    YuvPlayer *yuvPlayer;
    AVStream *inputVideoStream;
    AVFormatContext *afc_input;
    AVCodecContext *vCtxD;
    int videoStreamIndex;
    int width ;
    int height ;
    int buildInput(const char* inputPath);


    //output
    AVFormatContext *afc_output;
    AVCodecContext *vCtxE;
    AVCodecContext *aCtxE;
    AVStream *videoOutputStream ;
    AVStream *audioOutputStream ;
    int buildOutput(const char* outputPath);

public :
    VideoDub(const char *path, const  char *outputPath, ANativeWindow *win);
    int startDub();
    int setFlag(bool flag);
    void run();
    void update(MyData *mydata) ;
    void addVoice(char *pcm , int size);
    ~VideoDub();

};
#endif //MYFFMPEG_VIDEODUB_H
