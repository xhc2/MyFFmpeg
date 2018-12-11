#include <my_log.h>
#include "decode_test.h"

DecodeTest::DecodeTest(){
    afcOutput = NULL;
    buildOutput("sdcard/FFmpeg/testback.mp4");

    int width = 1280;
    int height = 720;
    int yuvSize = width * height * 3 / 2 ;
    int ySize = width * height;
    AVFrame *frame = av_frame_alloc();
    FILE *file = fopen("sdcard/FFmpeg/test.yuv" , "r");
    char *buffer = (char *)malloc(yuvSize);
    while(true){
        int len = fread(buffer , 1 , yuvSize , file);
        if(len != yuvSize){
            LOGE(" READ END ");
            break;
        }
        frame->data[0] = (uint8_t *)buffer ;
        frame->data[0] = (uint8_t *)buffer + ySize;
        frame->data[0] = (uint8_t *)buffer + ySize * 5 / 4;
    }
}









int DecodeTest::buildOutput(const char *outputPath) {
    int result = initOutput(outputPath, &afcOutput);
    if (result < 0) {
        LOGE(" initOutput faild !");
        return -1;
    }
    AVCodecParameters codecpar;
    codecpar.width = 1280;
    codecpar.height = 720;
    codecpar.format = AV_PIX_FMT_YUV420P;

    LOGE(" VIDEO WIDTH %d , VIDEO HEIGHT %d " ,codecpar.width , codecpar.height );
    result = addOutputVideoStream(afcOutput, &vCtxE, codecpar);
    if (result < 0 || vCtxE == NULL) {
        LOGE("addOutputVideoStream FAILD !");
        return -1;
    }
    videoOutputStreamIndex = result;
    result = writeOutoutHeader(afcOutput, outputPath);
    if (result < 0) {
        LOGE(" writeOutoutHeader FAILD !");
        return -1;
    }
    return 1;
}












