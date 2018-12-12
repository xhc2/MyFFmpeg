//
// Created by Administrator on 2018/10/25/025.
//

#ifndef MYFFMPEG_EDITPARENT_H
#define MYFFMPEG_EDITPARENT_H

#include <stdio.h>
#include <vector>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
};

#define DEBUG


using namespace std;

class EditParent {

public :
    static void custom_log(void *ptr, int level, const char *fmt, va_list vl);
    int getProgress();
//    int getVideoWidth(AVCodecParameters *coder);
//    int getVideoHeight(AVCodecParameters *coder);

protected:


    EditParent();
    //输入，解码 相关
    int videoStreamIndex;
    int audioStreamIndex;
    int open_input_file(const char *filename  , AVFormatContext **ps);
    int getVideoDecodeContext( AVFormatContext *ps,    AVCodecContext **dec_ctx); //获取视频解码器，并返回videoStreamIndex;
    int getAudioDecodeContext(AVFormatContext *ps,    AVCodecContext **dec_ctx); //获取音频解码器，并返回audioStreamIndex;
    AVFrame *decodePacket(AVCodecContext *decode , AVPacket *packet);
    AVPacket *encodeFrame(AVFrame *frame ,AVCodecContext *codecContext );
    int getVideoStreamIndex(AVFormatContext *fmt_ctx);
    int getAudioStreamIndex(AVFormatContext *fmt_ctx);

    //输出,编码 相关
    int videoOutputStreamIndex;
    int audioOutputStreamIndex;
    int outFrameRate;
    int initOutput(const char* ouput , AVFormatContext **ctx);
    int addOutputVideoStream(AVFormatContext *afc_output ,AVCodecContext **vCtxE , AVCodecParameters codecpar);
    int addOutputAudioStream(AVFormatContext *afc_output ,AVCodecContext **aCtxE , AVCodecParameters codecpar);
    int getVideoOutFrameRate();
    int writeOutoutHeader(AVFormatContext *afc_output , const char* outputPath);
    int parseVideoParams(int* params , int size , AVCodecParameters *codecpar);
    int getVideoOutputStreamIndex();
    int getAudioOutputStreamIndex();
    int  writeTrail(AVFormatContext *afc_output );
    ~EditParent();

    //进度
    int progress;

    AVRational timeBaseFFmpeg;
private :

};


#endif //MYFFMPEG_EDITPARENT_H
