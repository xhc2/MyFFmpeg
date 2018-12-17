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
#include <libswscale/swscale.h>
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
    queue<AVFrame *> frameQue;
    queue<AVFrame *> encodeFrameQue;
    int maxFrameSize ;
    int maxWidth ;
    int maxHeight ;
    SwsContext *sws;
    int64_t apts, vpts;
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
    char *audioBuffer ;
    int buildOutput(const char *outputPath);
    int initSwsContext(int inWidth, int inHeight, int inpixFmt);
    void destroySwsContext();
public :
    VideoDub( );
    int init(const char* intputpath  , const char* outputPath, ANativeWindow *win );
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
