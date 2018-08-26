//
// Created by Administrator on 2018/8/26/026.
//



#include "SRSLibRtmp.h"
#include <my_log.h>
SRSLibRtmp::SRSLibRtmp(){

}

SRSLibRtmp::~SRSLibRtmp(){

}

void SRSLibRtmp::test(const char* path){
    rtmp = srs_rtmp_create(path);
    LOGE(" create rtmp success ");
    LOGE("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());
    srs_rtmp_destroy(rtmp);
}