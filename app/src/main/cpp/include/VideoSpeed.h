//
// Created by Administrator on 2018/12/19/019.
//

#ifndef MYFFMPEG_VIDEOSPEED_H
#define MYFFMPEG_VIDEOSPEED_H

#include "MyThread.h"
#include "FilterParent.h"

class VideoSpeed : public MyThread , public FilterParent{

private :

    //input
    AVFormatContext *afc_input;
    AVCodecContext *aCtxD;
    AVCodecContext *vCtxD;
    AVStream *inputAudioStream ;
    AVStream *inputVideoStream ;

    int buildInput(const char*input);


    // output
    AVFormatContext *afc_output;
    AVCodecContext *aCtxE;
    AVCodecContext *vCtxE;
    AVStream *outputVideoStream;
    AVStream *outputAudioStream;
    int buildOutput(const char *output);
    int buildVideoFilter(float speed);

public :
    VideoSpeed();
    int init(const char* inputPath , float speed , const char* outputPath);
    int startSpeed();
    void run();
    ~VideoSpeed();
};

#endif //MYFFMPEG_VIDEOSPEED_H
