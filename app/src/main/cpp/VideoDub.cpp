//
// Created by Administrator on 2018/12/13/013.
//

#include <my_log.h>
#include "VideoDub.h"
#include "time.h"

VideoDub::VideoDub(const char *intputpath, const  char *outputPath,ANativeWindow *win) {

    afc_input = NULL;
    vCtxD = NULL;
    int result;
    result = buildInput(intputpath);
    if (result < 0) {
        LOGE(" BUILD INPUT FALILD !");
        return;
    }
    result = buildOutput(outputPath);
    if(result < 0){
        LOGE(" BUILD OUTPUT FAILD !");
        return ;
    }
    width = inputVideoStream->codecpar->width;
    height = inputVideoStream->codecpar->height;
    yuvPlayer = new YuvPlayer(win, width, height);
    setPause();
    this->addNotify(yuvPlayer);
}

int VideoDub::buildInput(const char *inputPath) {
    int result = open_input_file(inputPath, &afc_input);
    if (result < 0 || afc_input == NULL) {
        LOGE(" open_input_file faild ! ");
        return -1;
    }
    result = getVideoDecodeContext(afc_input, &vCtxD);
    if (result < 0) {
        LOGE(" getVideoDecodeContext faild !");
        return -1;
    }
    videoStreamIndex = result;
    inputVideoStream = afc_input->streams[videoStreamIndex];
    return 1;
}

int VideoDub::buildOutput(const char *outputPath) {
    afc_output = NULL;
    int result ;
    result = initOutput(outputPath , &afc_output);
    if(result < 0){
        LOGE(" initOutput faild ! ");
        return -1;
    }
    AVCodecParameters *vparams = avcodec_parameters_alloc();
    avcodec_parameters_copy(vparams , afc_input->streams[videoStreamIndex]->codecpar);
    result = addOutputVideoStream(afc_output , &vCtxE , *vparams);
    avcodec_parameters_free(&vparams);
    if(result < 0){
        LOGE(" addOutputVideoStream FAILD !");
        return -1;
    }
    videoOutputStreamIndex = result ;
    videoOutputStream = afc_output->streams[result];


    AVCodecParameters *aparams = avcodec_parameters_alloc();
    aparams->sample_rate = 44100;
    aparams->format = AV_SAMPLE_FMT_S16 ;
    aparams->channel_layout = AV_CH_LAYOUT_MONO ;
    aparams->channels = 1;
    result = addOutputAudioStream(afc_output , &aCtxE , *aparams);
    avcodec_parameters_free(&aparams);
    if(result < 0){
        LOGE(" addOutputAudioStream FAILD !");
        return -1;
    }
    result = writeOutoutHeader(afc_output , outputPath);
    if(result < 0){
        LOGE(" writeOutoutHeader faild ! ");
       return -1;
    }
    return -1;
}



int VideoDub::startDub() {
    AVPacket *pkt = av_packet_alloc();
    int size = width * height;
    int result;
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }

        threadSleep(20);
        result = av_read_frame(afc_input, pkt);

        if (result < 0) {
            LOGE(" read end !");
            break;
        }
        if (pkt->stream_index == videoStreamIndex) {
            AVFrame *vframe = decodePacket(vCtxD, pkt);
            av_packet_unref(pkt);
            if (vframe != NULL) {
                MyData *myData = new MyData();
                myData->pts = vframe->pts;
                myData->isAudio = false;
                myData->vWidth = width;
                myData->vHeight = height;

                myData->size = (vframe->linesize[0] + vframe->linesize[1] + vframe->linesize[2]) * vframe->height;

                //y
                myData->datas[0] = (uint8_t *) malloc(size);
                //u
                myData->datas[1] = (uint8_t *) malloc(size / 4);
                //v
                myData->datas[2] = (uint8_t *) malloc(size / 4);


                //把yuv数据读取出来
                for (int i = 0; i < height; ++i) {
                    memcpy(myData->datas[0] + width * i, vframe->data[0] + vframe->linesize[0] * i,
                           width);
                }

                for (int i = 0; i < height / 2; ++i) {
                    memcpy(myData->datas[1] + width / 2 * i,
                           vframe->data[1] + vframe->linesize[1] * i, width / 2);
                }

                for (int i = 0; i < height / 2; ++i) {
                    memcpy(myData->datas[2] + width / 2 * i,
                           vframe->data[2] + vframe->linesize[2] * i, width / 2);
                }
                LOGE(" AV_READ_FRAME !");
                this->notify(myData);
            }
        }

    }
    return 1;
}

int VideoDub::setFlag(bool flag) {
    flag ? setPlay() : setPause();
    return 1;
}

//添加声音
void VideoDub::addVoice(char *pcm, int size) {

}



void VideoDub::run() {
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
    }
    writeTrail(afc_output);
}

//这是被通知的方法
void VideoDub::update(MyData *mydata) {

}


VideoDub::~VideoDub() {

}

