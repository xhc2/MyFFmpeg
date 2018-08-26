//
// Created by Administrator on 2018/8/26/026.
//

#ifndef MYFFMPEG_SRSLIBRTMP_H
#define MYFFMPEG_SRSLIBRTMP_H

#include <stdio.h>

extern "C" {
#include "srs_librtmp.h"
};


class SRSLibRtmp {

private:
    srs_rtmp_t rtmp;
public :

    SRSLibRtmp();

    ~SRSLibRtmp();

    void test(const char *path);

};

#endif //MYFFMPEG_SRSLIBRTMP_H
