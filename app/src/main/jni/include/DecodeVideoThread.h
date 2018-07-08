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
}
class DecodeVideoThread : public MyThread  , public Notify{

private:
    int maxPackage = 100;
    queue<AVPacket *> videoPktQue;
    pthread_mutex_t mutex_pthread = PTHREAD_MUTEX_INITIALIZER;
    Utils util;
    AVFrame *vframe;

    AVCodecContext  *vc;
    AVFormatContext *afc;
    int videoIndex;
public :
    void update(MyData mydata);
    DecodeVideoThread(AVFormatContext *afc , AVCodecContext  *vc  ,int videoIndex);
    ~DecodeVideoThread();
    void run();
    int pts ;
    int apts;
};

#endif //MYFFMPEG_DECODEVIDEOTHREAD_H