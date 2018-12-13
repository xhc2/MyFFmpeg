//
// Created by Administrator on 2018/12/13/013.
//

#include <my_log.h>
#include "VideoDub.h"
#include "time.h"

VideoDub::VideoDub(const char *intputpath, const char *outputPath, ANativeWindow *win) {
    readEnd= false;
    afc_input = NULL;
    vCtxD = NULL;
    int result;
    result = buildInput(intputpath);
    if (result < 0) {
        LOGE(" BUILD INPUT FALILD !");
        return;
    }
    width = inputVideoStream->codecpar->width;
    height = inputVideoStream->codecpar->height;
    result = buildOutput(outputPath);
    if (result < 0) {
        LOGE(" BUILD OUTPUT FAILD !");
        return;
    }
    yuvPlayer = new YuvPlayer(win, width, height);

    this->addNotify(yuvPlayer);



    setPause();
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

    outChannel = 1;
    outChannelLayout = AV_CH_LAYOUT_MONO;
    sampleFormat = AV_SAMPLE_FMT_FLTP;
    sampleRate = 44100;
    nbSample = 1024;
    afc_output = NULL;
    apts = 0 ;
    vpts = 0;
    src_data = (uint8_t **)malloc(8 * sizeof(uint8_t *));
    int result = av_samples_alloc(src_data ,&src_linesize ,  outChannel , nbSample , sampleFormat ,0);
    if(result < 0){
        LOGE(" av_samples_alloc faild ! ");
        return -1;
    }
    result = initOutput(outputPath, &afc_output);
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return -1;
    }
    AVCodecParameters *vparams = avcodec_parameters_alloc();
    avcodec_parameters_copy(vparams, afc_input->streams[videoStreamIndex]->codecpar);
    result = addOutputVideoStream(afc_output, &vCtxE, *vparams);


    avcodec_parameters_free(&vparams);
    if (result < 0) {
        LOGE(" addOutputVideoStream FAILD !");
        return -1;
    }
    videoOutputStreamIndex = result;
    videoOutputStream = afc_output->streams[result];


    AVCodecParameters *aparams = avcodec_parameters_alloc();
    aparams->sample_rate = sampleRate;
    aparams->format = sampleFormat;
    aparams->channel_layout = outChannelLayout;
    aparams->channels = outChannel;
    result = addOutputAudioStream(afc_output, &aCtxE, *aparams);
    avcodec_parameters_free(&aparams);
    LOGE("  aCtxE SIZE %d " , aCtxE->frame_size);
    if (result < 0) {
        LOGE(" addOutputAudioStream FAILD !");
        return -1;
    }
    audioOutputStream = afc_output->streams[result];
    result = initSwrContext(1, AV_SAMPLE_FMT_S16, 44100);
    if (result < 0) {
        LOGE(" initSwrContext FAILD ! ");
        return -1;
    }

    audioOutBuffer = (uint8_t *) malloc(av_get_bytes_per_sample(sampleFormat) * nbSample);

    result = writeOutoutHeader(afc_output, outputPath);
    if (result < 0) {
        LOGE(" writeOutoutHeader faild ! ");
        return -1;
    }

    outAFrame = av_frame_alloc();
    outAFrame->sample_rate = sampleRate ;
    outAFrame->format = sampleFormat;
    outAFrame->channels = outChannel;
    outAFrame->channel_layout = outChannelLayout ;
    aCalDuration =  AV_TIME_BASE  / sampleRate;
    return 1;
}


int VideoDub::initSwrContext(int channelCount, AVSampleFormat in_sample_fmt, int in_sample_rate) {

    //这个地方很奇怪，这里就有用，不然会崩溃。
    swc = swr_alloc();
    if (swc == NULL) {
        LOGE(" SWC ALLOC FAILD !");
        return -1;
    }
    swc = swr_alloc_set_opts(swc,
                             outChannelLayout,
                             sampleFormat, sampleRate,
                             av_get_default_channel_layout(channelCount),
                             in_sample_fmt, in_sample_rate,
                             0, 0);

    if (swc == NULL) {
        LOGE(" swc NULL !");
        return -1;
    }
    if (swr_init(swc) < 0) {
        LOGE(" swr_init FAILD !");
        return -1;
    }
    LOGE(" initSwrContext SUCCESS !");

    return 1;
}

void VideoDub::destroySwrContext() {
    if (swc != NULL) {
        swr_free(&swc);
        swc = NULL;
    }
}


int VideoDub::startDub() {
    int result;
    this->start();
    int size = width * height;

    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
        threadSleep(10);
        AVPacket *pkt = av_packet_alloc();
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            av_packet_free(&pkt);
            readEnd = true;
            LOGE(" read end !");
            break;
        }
        if (pkt->stream_index == videoStreamIndex) {
            videoQue.push(pkt);
            AVFrame *vframe = decodePacket(vCtxD, pkt);
            if (vframe != NULL) {
                MyData *myData = new MyData();
                myData->pts = vframe->pts;
                myData->isAudio = false;
                myData->vWidth = width;
                myData->vHeight = height;

                myData->size = (vframe->linesize[0] + vframe->linesize[1] + vframe->linesize[2]) *
                               vframe->height;

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
                this->notify(myData);
            }
        }
        else{
            av_packet_free(&pkt);
        }
    }
    return 1;
}

int VideoDub::setFlag(bool flag) {
    flag ? setPlay() : setPause();
    return 1;
}

//FILE *pcmF = NULL;
//添加声音
void VideoDub::addVoice(char *pcm, int size) {
    memcpy(src_data[0] , pcm , size);
    int outNbSample = swr_convert(swc, &audioOutBuffer, nbSample ,
                                  (const uint8_t **) src_data, size / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16)) ;
    if(outNbSample < 0){
        LOGE(" AUDIO CONVERT FAILD !");
    }
    outAFrame->data[0] = src_data[0] ;
    audioCount += outNbSample;
    outAFrame->pts = audioCount * aCalDuration ;
    AVPacket *pkt = encodeFrame(outAFrame , aCtxE);
    if(pkt != NULL){
        av_packet_rescale_ts(pkt , timeBaseFFmpeg , audioOutputStream->time_base );
        audioQue.push(pkt);
    }
}


void VideoDub::run() {
    int result ;
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
        if (this->pause || videoOutputStream == NULL || audioOutputStream == NULL) {
            threadSleep(2);
            continue;
        }
        if (audioQue.size() <= 0 || videoQue.size() <= 0) {
            if (readEnd) {
                break;
            }
            threadSleep(2);
            continue;
        }

        AVPacket *aPkt = audioQue.front();
        AVPacket *vPkt = videoQue.front();
//        LOGE(" apts %lld , vpts %lld ", apts, vpts);
        if (av_compare_ts(apts, audioOutputStream->time_base, vpts, videoOutputStream->time_base) < 0) {
            LOGE(" write audio " );
            apts = aPkt->pts;
            result = av_interleaved_write_frame(afc_output, aPkt);
            if (result < 0) {
                LOGE(" audio av_interleaved_write_frame  faild ! %s " , av_err2str(result) );
            }
            av_packet_free(&aPkt);
            audioQue.pop();
        } else {
            LOGE(" write video " );
            av_packet_rescale_ts(vPkt ,inputVideoStream->time_base , videoOutputStream->time_base );
            vpts = vPkt->pts;
            result = av_interleaved_write_frame(afc_output, vPkt);
            if (result < 0) {
                LOGE(" video av_interleaved_write_frame faild ! %s " , av_err2str(result) );
            }
            av_packet_free(&vPkt);
            videoQue.pop();
        }

    }
    writeTrail(afc_output);
    LOGE("-------------------------------- ALL END --------------------------------");
}

//这是被通知的方法
void VideoDub::update(MyData *mydata) {

}


VideoDub::~VideoDub() {

}

