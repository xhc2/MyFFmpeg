//
// Created by Administrator on 2018/10/15/015.
//

#ifndef MYFFMPEG_VIDEOCLIP_H
#define MYFFMPEG_VIDEOCLIP_H
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/jni.h>
}
#include <my_log.h>
class VideoClip{

private :
    int startSecond ;
    int endSecond ;
    int video_index;
    int audio_index;
    AVStream *audioStream ;
    AVStream *videoStream ;
    AVCodec *videoCodecD;
    AVCodec *videoCodecE;

    AVCodecContext *vCtxD;
    AVCodecContext *vCtxE;
    AVFormatContext *afc_input ;
    AVFormatContext *afc_output ;
    AVOutputFormat *afot;
    int initInput();
    int initOutput();

    int addVideoOutputStream();
    int addAudioOutputStream();
    AVStream *audioOutStream ;
    AVStream *videoOutStream ;
    char* path;
    char* outputPath;

    bool findKeyFrame ;

public :
    VideoClip(const char* path , const char* output , int startSecond , int endSecond);
    void start();
    void encodeTest();
    ~VideoClip();


};

#endif //MYFFMPEG_VIDEOCLIP_H
