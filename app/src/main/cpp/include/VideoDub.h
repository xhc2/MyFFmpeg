//
// Created by Administrator on 2018/12/13/013.
//

#ifndef MYFFMPEG_VIDEODUB_H
#define MYFFMPEG_VIDEODUB_H

#include <android/native_window.h>
#include <queue>

#include "EditParent.h"
#include "MyThread.h"
#include "YuvPlayer.h"
#include "ReadAvPacket.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
};
using namespace std;
class VideoDub : public EditParent, public MyThread, public Notify {
private :
    bool readEnd ;
    YuvPlayer *yuvPlayer;
    AVStream *inputVideoStream;
    AVFormatContext *afc_input;
    AVCodecContext *vCtxD;
    int videoStreamIndex;
    int width;
    int height;
    ReadAVPackage *readAVPackage ;
    int buildInput(const char *inputPath);


    //output

    bool decodeEnd ;
    queue<AVPacket *> audioQue;
    queue<AVPacket *> videoQue;
    queue<MyData *> showVideoQue;
    int64_t apts, vpts;
    uint8_t **src_data;
    int src_linesize ;
    int audioCount ;
    int64_t  aCalDuration;
    int outChannel ;
    uint64_t outChannelLayout;
    AVSampleFormat sampleFormat ;
    AVFrame *outAFrame;
    int sampleRate;
    int nbSample ;
    AVFormatContext *afc_output;
    AVCodecContext *vCtxE;
    AVCodecContext *aCtxE;
    AVStream *videoOutputStream;
    AVStream *audioOutputStream;
    int buildOutput(const char *outputPath);
public :
    VideoDub(const char *path, const char *outputPath, ANativeWindow *win);

    int startDub();

    int setFlag(bool flag);

    void run();

    void update(MyData *mydata);

    void addVoice(char *pcm, int size);
    void destroyInput();
    void destroyOther();
    void destroyOutput();
    ~VideoDub();

};

#endif //MYFFMPEG_VIDEODUB_H
