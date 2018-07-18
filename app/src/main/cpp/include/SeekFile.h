//
// Created by Administrator on 2018/7/17/017.
//

#ifndef MYFFMPEG_SEEKFILE_H
#define MYFFMPEG_SEEKFILE_H

#include "MyThread.h"
#include "Utils.h"
extern "C"{
    #include <libavformat/avformat.h>
};
class SeekFile : public MyThread{

private:
    AVFormatContext *afc;
    int64_t seekPts;
    Utils utils;
public :
    SeekFile(AVFormatContext *afc);
    ~SeekFile();
    void seekStart();
    void seek(float progress , int streamIndex, bool isAudio);
    void findFrame(int64_t pts, int streamIndex);
    void run();

};


#endif //MYFFMPEG_SEEKFILE_H
