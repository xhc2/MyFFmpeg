//
// Created by Administrator on 2018/12/13/013.
//

#include <my_log.h>

#include "VideoDub.h"
#include "time.h"

/**
 * video packet 和 audio packet 不能直接写入多媒体文件中。
 * 因为编码器和解码器可能不同。
 * 用h264编码的packet不能直接写入mpeg4对应的文件中。
 */


VideoDub::VideoDub(const char *intputpath, const char *outputPath, ANativeWindow *win) {
    readEnd = false;
    decodeEnd = false;
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
    int audioStreamIndex = getAudioStreamIndex(afc_input) ;
    LOGE(" input audio index %d , video index %d " , audioStreamIndex , videoStreamIndex);
    readAVPackage = new ReadAVPackage(afc_input,audioStreamIndex , videoStreamIndex);
    this->addNotify(yuvPlayer);
    readAVPackage->addNotify(this);

    setFlag(false);
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
    int result = 0;
    outChannel = 1;
    outChannelLayout = AV_CH_LAYOUT_MONO;
    sampleFormat = AV_SAMPLE_FMT_S16;//AV_SAMPLE_FMT_FLTP;
    sampleRate = 44100;

    afc_output = NULL;
    apts = 0;
    vpts = 0;

    result = initOutput(outputPath, &afc_output);
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return -1;
    }
    result = addOutputVideoStream(afc_output, &vCtxE, *inputVideoStream->codecpar);
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
    LOGE("  aCtxE SIZE %d ", aCtxE->frame_size);
    if (result < 0) {
        LOGE(" addOutputAudioStream FAILD !");
        return -1;
    }
    audioOutputStreamIndex = result;
    audioOutputStream = afc_output->streams[result];

    result = writeOutoutHeader(afc_output, outputPath);
    if (result < 0) {
        LOGE(" writeOutoutHeader faild ! ");
        return -1;
    }
    nbSample = aCtxE->frame_size;
    src_data = (uint8_t **) malloc(8 * sizeof(uint8_t *));
    result = av_samples_alloc(src_data, &src_linesize, outChannel, nbSample, sampleFormat, 0);
    if (result < 0) {
        LOGE(" av_samples_alloc faild ! ");
        return -1;
    }
    outAFrame = av_frame_alloc();
    outAFrame->sample_rate = sampleRate;
    outAFrame->format = sampleFormat;
    outAFrame->channels = outChannel;
    outAFrame->channel_layout = outChannelLayout;
    outAFrame->nb_samples = nbSample;
    result = av_frame_make_writable(outAFrame);
    if (result < 0) {
        LOGE(" av_frame_make_writable %s  ", av_err2str(result));
    }
    aCalDuration = AV_TIME_BASE / sampleRate;

    return 1;
}


int VideoDub::startDub() {
//    this->start();
    readAVPackage->start();
    int size = width * height;

    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }

        if (showVideoQue.size() <= 0) {
            continue;
        }
        LOGE("SHOW VIDEO QUE %d " , showVideoQue.size());
        MyData *myDataP = showVideoQue.front();
        AVPacket *pkt = myDataP->pkt;
        AVFrame *vframe = decodePacket(vCtxD, pkt);
        delete myDataP;
        if (vframe != NULL) {
            MyData *myData = new MyData();
            myData->pts = vframe->pts;
            myData->isAudio = false;
            myData->vWidth = width;
            myData->vHeight = height;
            myData->size = (vframe->linesize[0] + vframe->linesize[1] + vframe->linesize[2]) *
                           vframe->height;
            myData->datas[0] = (uint8_t *) malloc(size);
            myData->datas[1] = (uint8_t *) malloc(size / 4);
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
            AVPacket *vPkt = encodeFrame(vframe, vCtxE);
            av_frame_free(&vframe);
            if (vPkt != NULL) {
                videoQue.push(vPkt);
            }
        }
    }
    decodeEnd = true;
    return 1;
}

int VideoDub::setFlag(bool flag) {
    if (flag) {
        readAVPackage->setPlay();
        setPlay();
    } else {
        readAVPackage->setPause();
        setPause();

    }

    return 1;
}

//添加声音 adpcm_swf
void VideoDub::addVoice(char *pcm, int size) {
    if (readEnd) {
        return;
    }
    memcpy(src_data[0], pcm, size);
    outAFrame->data[0] = src_data[0];
    outAFrame->linesize[0] = size;
    audioCount += size / av_get_bytes_per_sample(sampleFormat);
    outAFrame->pts = audioCount * aCalDuration;
    AVPacket *pkt = encodeFrame(outAFrame, aCtxE);
    if (pkt != NULL) {
        audioQue.push(pkt);
    }
}


void VideoDub::run() {
    int result;
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
        if (this->pause || videoOutputStream == NULL || audioOutputStream == NULL) {
            threadSleep(2);
            continue;
        }
//        LOGE(" video %d , audio %d ", videoQue.size(), audioQue.size());
        if (audioQue.size() <= 0 || videoQue.size() <= 0) {
            if (readEnd) {
                break;
            }
            threadSleep(2);
            continue;
        }
        AVPacket *aPkt = audioQue.front();
        AVPacket *vPkt = videoQue.front();
        if (av_compare_ts(apts, audioOutputStream->time_base, vpts, videoOutputStream->time_base) <
            0) {
            aPkt->stream_index = audioOutputStreamIndex;
            av_packet_rescale_ts(aPkt, timeBaseFFmpeg, audioOutputStream->time_base);
            apts = aPkt->pts;
            result = av_interleaved_write_frame(afc_output, aPkt);
            if (result < 0) {
                LOGE(" audio av_interleaved_write_frame  faild ! %s ", av_err2str(result));
            }
            av_packet_free(&aPkt);
            audioQue.pop();
        } else {
            vPkt->stream_index = videoOutputStreamIndex;
            av_packet_rescale_ts(vPkt, inputVideoStream->time_base, videoOutputStream->time_base);
            vpts = vPkt->pts;
            result = av_interleaved_write_frame(afc_output, vPkt);
            if (result < 0) {
                LOGE(" video av_interleaved_write_frame faild ! %s ", av_err2str(result));
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

    if (mydata == NULL ) {
        return;
    }
    if(mydata->isAudio){
        delete mydata;
        return;
    }
   while( !isExit ){
       if (showVideoQue.size() < 100) {
           showVideoQue.push(mydata);
           break;
       }
       else{
           threadSleep(2);
       }
   }

}

void VideoDub::destroyInput() {
    if (vCtxD != NULL) {
        avcodec_free_context(&vCtxD);
        vCtxD = NULL;
    }
    if (afc_input != NULL) {
        avformat_free_context(afc_input);
        afc_input = NULL;
    }
    delete yuvPlayer;
}

void VideoDub::destroyOutput() {
    if (vCtxE != NULL) {
        avcodec_free_context(&vCtxE);
        vCtxE = NULL;
    }
    if (aCtxE != NULL) {
        avcodec_free_context(&aCtxE);
        aCtxE = NULL;
    }
    if (afc_output != NULL) {
        avformat_free_context(afc_output);
        afc_output = NULL;
    }
    if (outAFrame != NULL) {
        av_frame_free(&outAFrame);
    }
    if (src_data != NULL) {
        for (int i = 0; i < 8; ++i) {
            av_freep(src_data[i]);
        }
    }
}

void VideoDub::destroyOther() {
    while (!audioQue.empty()) {
        AVPacket *pkt = audioQue.front();
        if (pkt != NULL) {
            av_packet_free(&pkt);
        }
        audioQue.pop();
    }
    while (!videoQue.empty()) {
        AVPacket *pkt = videoQue.front();
        if (pkt != NULL) {
            av_packet_free(&pkt);
        }
        videoQue.pop();
    }
}

VideoDub::~VideoDub() {
    this->stop();
    while (!decodeEnd) {
        LOGE(" wait for end!");
        threadSleep(1);
    }
    this->join();

    destroyOther();
    destroyInput();
    destroyOutput();
}

