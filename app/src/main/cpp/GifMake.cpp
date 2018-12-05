//
// Created by Administrator on 2018/12/5/005.
//

#include <my_log.h>
#include "GifMake.h"

GifMake::GifMake(const char *inputPath, const char *outPath) {
    int result ;
    result = initOutput(outPath , &afc_output);
    if(result < 0){
        LOGE(" initOutput faild !");
        return ;
    }
    AVOutputFormat *afot = afc_output->oformat;
    AVCodec *videoCodecE = avcodec_find_encoder(afot->video_codec);
    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return;
    }
    LOGE("video ENCODE NAME %s ", videoCodecE->name);
    AVCodecContext *vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return;
    }
    gif_encode_init(vCtxE);
//    result = addOutputVideoStream(afc_output , NULL , );
}






GifMake::~GifMake() {

}

