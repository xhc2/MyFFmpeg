//
// Created by Administrator on 2018/12/18/018.
//

#ifndef MYFFMPEG_VIDEOMUSIC_H
#define MYFFMPEG_VIDEOMUSIC_H

#include "MyThread.h"
#include "EditParent.h"

class VideoMusic :  public EditParent{

private :
    bool isExit;
    bool dealEnd ;
    //input
    AVFormatContext *afc_input_video;
    AVFormatContext *afc_input_audio;
    AVCodecContext *aCtxD;
    AVCodecContext *vCtxD;
    AVStream *audioInputStream ;
    AVStream *videoInputStream ;
    int64_t duration ;
    int buildVideoInput(const char* inputPath);
    int buildAudioInput(const char* inputPath);


    //output
    int audioSampleCount ;
    int64_t aDuration ;
    AVCodecContext *aCtxE;
    AVCodecContext *vCtxE;
    AVFormatContext *afc_output;
    AVStream *audioOutStream ;
    AVStream *videoOutStream ;
    int64_t apts , vpts ;
    int buildOutput(const char* output);
public :
    VideoMusic();
    int init(const char* inputPath , const char* musicPath , const char* outputPath);
    int startVideoMusic();
    void destroyInput();
    void destroyOutput();
    ~VideoMusic();
};
#endif //MYFFMPEG_VIDEOMUSIC_H
