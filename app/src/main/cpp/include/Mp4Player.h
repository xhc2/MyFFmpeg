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
#include "SeekFile.h"
#include "CallJava.h"
#include "NewAudioPlayer.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
class Mp4Player : public MyThread{

private :

    //用于显示yuv图像
    AVFormatContext *afc;
    AVCodecContext *ac, *vc;
    int video_index  ;
    int audio_index ;
    AVCodec *videoCode, *audioCode;

    int outWidth , outHeight ;
    int simpleRate ;
    int outChannel ;
    int64_t videoDuration;
    int initFFmpeg(const char* path);
    ReadAVPackage *readAVPackage;
    DecodeVideoThread *decodeVideo;
    DeocdeMyAudioThread *decodeAudio;
//    AudioPlayer *audioPlayer;
    NewAudioPlayer *audioPlayer;
    YuvPlayer *yuvPlayer;
    void run();
    SeekFile *seekFile;
    CallJava *cj;
public :
    Mp4Player(const char* playPath , ANativeWindow* win ,  CallJava *cj);
    void pauseVA();
    void clearAllQue();
    void changeSpeed(float speed);
    void playVA();
    void seekStart();
    void seek(float progress);
    ~Mp4Player();
    int getProgress();
    float getDuration();
    int getVideoWidth();
    int getVideoHeight();
};

#endif //MYFFMPEG_MP4PLAYER_H
