//
// Created by Administrator on 2018/7/17/017.
//

#ifndef MYFFMPEG_SEEKFILE_H
#define MYFFMPEG_SEEKFILE_H

#include "MyThread.h"
#include "Utils.h"
#include <queue>
extern "C"{
    #include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
};
using namespace std;
class SeekFile : public MyThread{

private:
    AVFormatContext *afc;
    int64_t seekPts;
    Utils utils;
    int audioIndex;
    int videoIndex;
//    queue<AVPacket *> audioTemp ;
public :
    SeekFile(AVFormatContext *afc, int audioIndex , int videoIndex);
    ~SeekFile();
    void seekStart();
    void seek(float progress );
    void  findFrame(int64_t pts );
    void run();

};


#endif //MYFFMPEG_SEEKFILE_H
