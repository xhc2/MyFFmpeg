//
// Created by Administrator on 2018/8/26/026.
//

#ifndef MYFFMPEG_SRSLIBRTMP_H
#define MYFFMPEG_SRSLIBRTMP_H

#include <stdio.h>
#include "MyThread.h"

extern "C" {
#include "srs_librtmp.h"
};


class SRSLibRtmp : public MyThread{

private:
    srs_rtmp_t rtmp;

public :

    SRSLibRtmp();

    ~SRSLibRtmp();
    void publish(const char* path);
    void rtmpDestroy();
    void test(const char *path);
    void publishH264(const char* path);
    int read_h264_frame(char* data, int size, char** pp, int* pnb_start_code, int fps,
                        char** frame, int* frame_size, int* dts, int* pts);
    void run();

};

#endif //MYFFMPEG_SRSLIBRTMP_H
