//
// Created by Administrator on 2018/7/3/003.
//

#ifndef MYFFMPEG_DECODEVIDEOTHREAD_H
#define MYFFMPEG_DECODEVIDEOTHREAD_H

#include "MyThread.h"
#include "Notify.h"
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
    void run();

public :
    void update(MyData mydata);
};

#endif //MYFFMPEG_DECODEVIDEOTHREAD_H
