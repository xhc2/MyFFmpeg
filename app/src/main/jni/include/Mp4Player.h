//
// Created by dugang on 2018/7/3.
//

#ifndef MYFFMPEG_MP4PLAYER_H
#define MYFFMPEG_MP4PLAYER_H

#include <jni.h>
#include "ReadAvPacket.h"
#include "DecodeVideoThread.h"
#include "DecodeMyAudioThread.h"
#include "AudioPlayer.h"
#include "YuvPlayer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
class Mp4Player : public MyThread{

private :
    const char* playPath;
    //用于显示yuv图像
    AVFormatContext *afc;
    int video_index = -1;
    int audio_index = -1;
    AVCodec *videoCode, *audioCode;
    AVCodecContext *ac, *vc;
    int outWidth , outHeight ;
    int simpleRate ;
//    int outChannel = 1;
    int64_t videoDuration;
    int initFFmpeg();
//    ReadAVPackage *readAVPackage;
    DecodeVideoThread *decodeVideo;
//    DeocdeMyAudioThread *decodeAudio;
    AudioPlayer *audioPlayer;
//    YuvPlayer *yuvPlayer;
public :
    Mp4Player(const char* playPath , ANativeWindow* win);
    ~Mp4Player();
    void run();
};

#endif //MYFFMPEG_MP4PLAYER_H
