//
// Created by dugang on 2018/7/3.
//

#ifndef MYFFMPEG_MP4PLAYER_H
#define MYFFMPEG_MP4PLAYER_H

#include <jni.h>
#include "MyWindow.h"
#include "MyAudio.h"
#include "ReadAvPacket.h"
#include "DecodeVideoThread.h"
#include "DecodeMyAudioThread.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
class Mp4Player{

private :
    const char* playPath;
    //用于显示yuv图像
    MyWindow *myWindow ;
    AVFrame *aframe;
    AVFrame *vframe;
    AVFrame *vframe_seek;
    AVFormatContext *afc;
    int video_index = -1;
    int audio_index = -1;
    AVCodec *videoCode, *audioCode;
    AVCodecContext *ac, *vc;
    int outWidth , outHeight ;
//视频总长度
    int64_t videoDuration;
    MyAudio *myAudio;
    int initFFmpeg();
    ReadAVPackage *readAVPackage;
    DecodeVideoThread *decodeVideo;
    DeocdeMyAudioThread *decodeAudio;

public :
    Mp4Player(const char* playPath , ANativeWindow* win);

    ~Mp4Player();

};

#endif //MYFFMPEG_MP4PLAYER_H
