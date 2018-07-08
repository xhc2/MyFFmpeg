//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_FFMPEGREADFRAME_H
#define MYFFMPEG_FFMPEGREADFRAME_H


#include <queue>

#include "MyThread.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
using namespace std;
class ReadFrame : public MyThread {

private :
    queue<AVPacket *> *audioPktQue;
    int maxAudioPacket ;
    AVFormatContext *afc;
    int audioIndex;
public :
    void run();

    ReadFrame(queue<AVPacket *> *queue, AVFormatContext *afc, int audioIndex);

    ~ReadFrame();
};

#endif //MYFFMPEG_FFMPEGREADFRAME_H
