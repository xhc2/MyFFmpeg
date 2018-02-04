//
// Created by Administrator on 2018/2/4/004.
//

#ifndef MYFFMPEG_MY_NEW_CAMERA_MUXER_FILTER_H
#define MYFFMPEG_MY_NEW_CAMERA_MUXER_FILTER_H
int init_camera_filter(const char* outputPath  , int w , int h , int aSize);
int initMuxerVideo_filter();
int initMuxerAudio_filter();
int init_muxer_Sws_filter();
int encode_filter(jbyte *nativeYuv , jbyte *nativePcm);
int encodeYuv__filter(jbyte *nativeYuv);
int encodePcm__filter(jbyte *nativePcm);
int encodeCamera_muxer_filter(jbyte *nativeYuv);
int encodeAudio_muxer_filter(jbyte *nativePcm);
int close_muxer();
int interleaved_write_filter(AVPacket *yuvPkt, AVPacket *pcmPkt);
#endif //MYFFMPEG_MY_NEW_CAMERA_MUXER_FILTER_H
