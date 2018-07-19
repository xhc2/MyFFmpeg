//
// Created by Administrator on 2018/7/3/003.
//

#ifndef MYFFMPEG_READAVPACKET_H
#define MYFFMPEG_READAVPACKET_H

#include "MyThread.h"
#include "Notify.h"
#include <queue>
extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}


class ReadAVPackage : public MyThread ,public Notify{

private :
    void run();

    int audioIndex;
    int videoIndex;
    AVFormatContext *afc;
public :
    ReadAVPackage(AVFormatContext *afc  , int audioIndex , int videoIndex);
    ~ReadAVPackage();
    void update(MyData *mydata);
};

#endif //MYFFMPEG_READAVPACKET_H
