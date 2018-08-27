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
void SRSLibRtmp::rtmpDestroy(){
    srs_rtmp_destroy(rtmp);
}

void SRSLibRtmp::publish(const char* path){
    LOGE("publish rtmp stream to server like FMLE/FFMPEG/Encoder\n");
    LOGE("srs(ossrs) client librtmp library.\n");
    LOGE("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());


    // @see: https://github.com/ossrs/srs/issues/126
    LOGE("rtmp url: %s", path);
    rtmp = srs_rtmp_create(path);

    if (srs_rtmp_handshake(rtmp) != 0) {
        LOGE("simple handshake failed.");
        rtmpDestroy();
        return ;
    }
    LOGE("simple handshake success");

    if (srs_rtmp_connect_app(rtmp) != 0) {
        LOGE("connect vhost/app failed.");
        rtmpDestroy();
        return ;
    }
    LOGE("connect vhost/app success");

    if (srs_rtmp_publish_stream(rtmp) != 0) {
        LOGE("publish stream failed.");
        rtmpDestroy();
        return ;
    }
    LOGE("publish stream success");

    u_int32_t timestamp = 0;
//    for (;;) {
//        char type = SRS_RTMP_TYPE_VIDEO;
//        int size = 4096;
//        char* data = (char*)malloc(4096);
//
//        timestamp += 40;
//
//        if (srs_rtmp_write_packet(rtmp, type, timestamp, data, size) != 0) {
//            rtmpDestroy();
//            break;
//        }
//        LOGE("sent packet: type=%s, time=%d, size=%d",
//                        srs_human_flv_tag_type2string(type), timestamp, size);
//
//        threadSleep(40);
//    }
    srs_rtmp_destroy(rtmp);
}

void SRSLibRtmp::run(){

}