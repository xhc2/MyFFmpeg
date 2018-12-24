//
// Created by Administrator on 2018/8/26/026.
//

#include "SRSLibRtmp.h"
#include <my_log.h>
#include <stdlib.h>
#include <fcntl.h>
#include <aacparse.h>


SRSLibRtmp::SRSLibRtmp() {
    rtmp = NULL;
}

SRSLibRtmp::~SRSLibRtmp() {

}

void SRSLibRtmp::test(const char *path) {
    //test mp4 file 相关
    int ret = 0;
    rtmp = srs_rtmp_create(path);

    if (srs_rtmp_handshake(rtmp) != 0) {
        LOGE("simple handshake failed.");
        rtmpDestroy();
        return;
    }
    LOGE("simple handshake success");
    if (srs_rtmp_connect_app(rtmp) != 0) {
        LOGE("connect vhost/app failed.");
        rtmpDestroy();
        return;
    }
    LOGE("connect vhost/app success");

    if (srs_rtmp_publish_stream(rtmp) != 0) {
        LOGE("publish stream failed.");
        rtmpDestroy();
        return;
    }
    AACParse *aac = new AACParse("sdcard/FFmpeg/music_src/test.aac");

    int64_t count = 0 ;
    int64_t pts = 0;
   while(true){
       AACFrame *frame = aac->getAACFrame(false);
       if(frame == NULL){
            break;
       }
       srs_bool flag = srs_aac_is_adts(frame->data , frame->size);
       count += frame->size / 4;
       pts = count * 22;
       LOGE(" PTS %lld " , pts);
       srs_audio_write_raw_frame(rtmp ,10 , 3 , 1 , 1 , frame->data , frame->size , pts );
   }

    delete aac;
    aac = NULL ;
    srs_rtmp_destroy(rtmp);
}

void SRSLibRtmp::rtmpDestroy() {
    if(rtmp != NULL){
        srs_rtmp_destroy(rtmp);
    }
}



void SRSLibRtmp::publish(const char *path) {
    LOGE("publish rtmp stream to server like FMLE/FFMPEG/Encoder\n");
    LOGE("srs(ossrs) client librtmp library.\n");
    LOGE("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());

    // @see: https://github.com/ossrs/srs/issues/126
    LOGE("rtmp url: %s", path);
    rtmp = srs_rtmp_create(path);

    if (srs_rtmp_handshake(rtmp) != 0) {
        LOGE("simple handshake failed.");
        rtmpDestroy();
        return;
    }
    LOGE("simple handshake success");
    if (srs_rtmp_connect_app(rtmp) != 0) {
        LOGE("connect vhost/app failed.");
        rtmpDestroy();
        return;
    }
    LOGE("connect vhost/app success");

//    if (srs_rtmp_publish_stream(rtmp) != 0) {
//        LOGE("publish stream failed.");
//        rtmpDestroy();
//        return;
//    }
    LOGE("publish stream success");

//    u_int32_t timestamp = 0;
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

int SRSLibRtmp::read_h264_frame(char *data, int size, char **pp, int *pnb_start_code, int fps,
                                char **frame, int *frame_size, int *dts, int *pts) {
    char *p = *pp;

    // @remark, for this demo, to publish h264 raw file to SRS,
    // we search the h264 frame from the buffer which cached the h264 data.
    // please get h264 raw data from device, it always a encoded frame.
    if (!srs_h264_startswith_annexb(p, size - (p - data), pnb_start_code)) {
        LOGE("h264 raw data invalid.");
        return -1;
    }

    // @see srs_write_h264_raw_frames
    // each frame prefixed h.264 annexb header, by N[00] 00 00 01, where N>=0,
    // for instance, frame = header(00 00 00 01) + payload(67 42 80 29 95 A0 14 01 6E 40)
    *frame = p;
    p += *pnb_start_code;

    for (; p < data + size; p++) {
        if (srs_h264_startswith_annexb(p, size - (p - data), NULL)) {
            break;
        }
    }

    *pp = p;
    *frame_size = p - *frame;
    if (*frame_size <= 0) {
        LOGE("h264 raw data invalid.");
        return -1;
    }

    // @remark, please get the dts and pts from device,
    // we assume there is no B frame, and the fps can guess the fps and dts,
    // while the dts and pts must read from encode lib or device.
    *dts += 1000 / fps;
    *pts = *dts;

    return 0;
}

void SRSLibRtmp::publishH264(const char *path) {

    LOGE("publish raw h.264 as rtmp stream to server like FMLE/FFMPEG/Encoder\n");
    LOGE("SRS(ossrs) client librtmp library.\n");
    LOGE("version: %d.%d.%d\n", srs_version_major(), srs_version_minor(), srs_version_revision());

    LOGE("     h264_raw_file: the h264 raw steam file.\n");
    LOGE("     rtmp_publish_url: the rtmp publish url.\n");
    LOGE("     fps: the video average fps, for example, 25.\n");
    LOGE(" For example:\n ");
    LOGE(" Where the file: http://winlinvip.github.io/srs.release/3rdparty/720p.h264.raw\n ");
    LOGE("     See: https://github.com/ossrs/srs/issues/66\n");


    const char *raw_file = "sdcard/FFmpeg/720p.h264.raw";
    const char *rtmp_url = path;
    // @remark, the dts and pts if read from device, for instance, the encode lib,
    // so we assume the fps is 25, and each h264 frame is 1000ms/25fps=40ms/f.
    double fps = 25.0;
    LOGE("raw_file=%s, rtmp_url=%s, fps=%.2f", raw_file, rtmp_url, fps);

    // open file
    int raw_fd = open(raw_file, O_RDONLY);
    if (raw_fd < 0) {
        LOGE("open h264 raw file %s failed.", raw_file);
        rtmpDestroy();
    }

    off_t file_size = lseek(raw_fd, 0, SEEK_END);
    if (file_size <= 0) {
        LOGE("h264 raw file %s empty.", raw_file);
        rtmpDestroy();
    }
    LOGE("read entirely h264 raw file, size=%dKB", (int) (file_size / 1024));

    char *h264_raw = (char *) malloc(file_size);
    if (!h264_raw) {
        LOGE("alloc raw buffer failed for file %s.", raw_file);
        rtmpDestroy();
    }

    lseek(raw_fd, 0, SEEK_SET);
    ssize_t nb_read = 0;
    if ((nb_read = read(raw_fd, h264_raw, file_size)) != file_size) {
        LOGE("buffer %s failed, expect=%dKB, actual=%dKB.",
                        raw_file, (int) (file_size / 1024), (int) (nb_read / 1024));
        rtmpDestroy();
    }

    // connect rtmp context
    rtmp = srs_rtmp_create(rtmp_url);

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

    int dts = 0;
    int pts = 0;
    // @remark, to decode the file.
    char *p = h264_raw;
    int count = 0;
    for (; p < h264_raw + file_size;) {
        // @remark, read a frame from file buffer.
        char *data = NULL;
        int size = 0;
        int nb_start_code = 0;
        if (read_h264_frame(h264_raw, (int) file_size, &p, &nb_start_code, fps, &data, &size, &dts,
                            &pts) < 0) {
            LOGE("read a frame from file buffer failed.");
            rtmpDestroy();
            return ;
        }

        // send out the h264 packet over RTMP
        int ret = srs_h264_write_raw_frames(rtmp, data, size, dts, pts);
        if (ret != 0) {
            if (srs_h264_is_dvbsp_error(ret)) {
                LOGE("ignore drop video error, code=%d", ret);
            } else if (srs_h264_is_duplicated_sps_error(ret)) {
                LOGE("ignore duplicated sps, code=%d", ret);
            } else if (srs_h264_is_duplicated_pps_error(ret)) {
                LOGE("ignore duplicated pps, code=%d", ret);
            } else {
                LOGE("send h264 raw data failed. ret=%d", ret);
                rtmpDestroy();
                return ;
            }
        }

        // 5bits, 7.3.1 NAL unit syntax,
        // H.264-AVC-ISO_IEC_14496-10.pdf, page 44.
        //  7: SPS, 8: PPS, 5: I Frame, 1: P Frame, 9: AUD, 6: SEI
        u_int8_t nut = (char) data[nb_start_code] & 0x1f;
        LOGE("sent packet: type=%s, time=%d, size=%d, fps=%.2f, b[%d]=%#x(%s)",
                        srs_human_flv_tag_type2string(SRS_RTMP_TYPE_VIDEO), dts, size, fps,
                        nb_start_code, (char) data[nb_start_code],
                        (nut == 7 ? "SPS" : (nut == 8 ? "PPS" : (nut == 5 ? "I" : (nut == 1 ? "P" : ( nut == 9 ? "AUD" : (nut == 6
                                                                                              ? "SEI"
                                                                                              : "Unknown")))))));

        // @remark, when use encode device, it not need to sleep.
        if (count++ == 9) {
            av_usleep(1000 * count / fps);
            count = 0;
        }
    }
    LOGE("h264 raw data completed");

    srs_rtmp_destroy(rtmp);
    close(raw_fd);
    free(h264_raw);

    return;

}

void SRSLibRtmp::run() {

}