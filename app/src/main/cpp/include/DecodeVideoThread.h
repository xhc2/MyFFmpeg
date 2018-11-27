//
// Created by Administrator on 2018/7/3/003.
//

#ifndef MYFFMPEG_DECODEVIDEOTHREAD_H
#define MYFFMPEG_DECODEVIDEOTHREAD_H

#include "MyThread.h"
#include "Notify.h"
#include "Utils.h"
#include <queue>
extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}
class DecodeVideoThread : public MyThread  , public Notify{

private:
    int maxPackage ;
    queue<AVPacket *> videoPktQue;
    pthread_mutex_t mutex_pthread ;
    Utils util;
    AVFrame *vframe;
    AVCodecContext  *vc;
    AVFormatContext *afc;
    int videoIndex;
    FILE *file ;
    bool finishFlag ;
public :
    void update(MyData *mydata);
    DecodeVideoThread(AVFormatContext *afc , AVCodecContext  *vc  ,int videoIndex);
    ~DecodeVideoThread();
    void run();
    void clearQue();
    int64_t pts ;
    int64_t apts;
};

#endif //MYFFMPEG_DECODEVIDEOTHREAD_H
