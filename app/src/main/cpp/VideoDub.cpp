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


VideoDub::VideoDub() {
    LOGE(" --------------------- VIDEO DUB START ---------------------");
    readEnd = false;
    decodeEnd = false;
    afc_input = NULL;
    sws = NULL;
    vCtxD = NULL;
    maxWidth = 640;
    maxHeight = 480;
    maxFrameSize = 10;
    audioCount = 0;
    readAVPackage = NULL ;
}

int VideoDub::init(const char *intputpath, const char *outputPath, ANativeWindow *win) {
    int result;
    result = buildInput(intputpath);
    if (result < 0) {
        LOGE(" BUILD INPUT FALILD !");
        return -1;
    }
    width = inputVideoStream->codecpar->width;
    height = inputVideoStream->codecpar->height;
    if (width > maxWidth || height > maxHeight) {
        //注意比例变化
        LOGE(" WH change  width %d  , height %d", width, height);
        height = (int) ((float) height / width * maxWidth);
        width = maxWidth;
        initSwsContext(inputVideoStream->codecpar->width, inputVideoStream->codecpar->height,
                       inputVideoStream->codecpar->format);
    }
    LOGE(" width %d , height %d ", width, height);
    result = buildOutput(outputPath);
    if (result < 0) {
        LOGE(" BUILD OUTPUT FAILD !");
        return -1;
    }
    yuvPlayer = new YuvPlayer(win, width, height);
    int audioStreamIndex = getAudioStreamIndex(afc_input);
    LOGE(" input audio index %d , video index %d ", audioStreamIndex, videoStreamIndex);
    readAVPackage = new ReadAVPackage(afc_input, audioStreamIndex, videoStreamIndex);
    this->addNotify(yuvPlayer);
    readAVPackage->addNotify(this);
    setFlag(false);
    return 1;
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
    int64_t duration = afc_input->duration;
    double second = duration * av_q2d(timeBaseFFmpeg);
    if (second > 90) {
        LOGE(" video > 90 ");

        return -1;
    }
    return 1;
}


int VideoDub::buildOutput(const char *outputPath) {
    int result = 0;
    outChannel = 1;
    outChannelLayout = AV_CH_LAYOUT_MONO;
    sampleFormat = AV_SAMPLE_FMT_S16;
    sampleRate = 44100;

    afc_output = NULL;
    apts = 0;
    vpts = 0;


    result = initOutput(outputPath, &afc_output);
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return -1;
    }
    AVCodecParameters *vparams = avcodec_parameters_alloc();
    avcodec_parameters_copy(vparams, inputVideoStream->codecpar);
    vparams->width = width;
    vparams->height = height;
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
    outAFrame = av_frame_alloc();
    outAFrame->sample_rate = sampleRate;
    outAFrame->format = sampleFormat;
    outAFrame->channels = outChannel;
    outAFrame->channel_layout = outChannelLayout;
    outAFrame->nb_samples = nbSample;
    aCalDuration = AV_TIME_BASE / sampleRate;
    audioBuffer = (char *)malloc(nbSample * av_get_bytes_per_sample(sampleFormat));
    return 1;
}


int VideoDub::startDub() {
    this->start();
    readAVPackage->start();
    int size = width * height;
    isExit = false;
    while (!isExit) {
        if (pause) {
            threadSleep(2);
            continue;
        }
        if (frameQue.size() <= 0) {
            continue;
        }
        threadSleep(40);
        AVFrame *vframe = frameQue.front();
        encodeFrameQue.push(vframe);
        frameQue.pop();
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
        }

    }
    decodeEnd = true;
    return 1;
}

int VideoDub::initSwsContext(int inWidth, int inHeight, int inpixFmt) {

    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, width, height,
                         AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {
        return -1;
    }
    LOGE(" initSwsContext SUCCESS ");
    return 1;
}

void VideoDub::destroySwsContext() {
    if (sws != NULL) {
        sws_freeContext(sws);
        sws = NULL;
    }
}


int VideoDub::setFlag(bool flag) {
    if (flag) {
        if(readAVPackage != NULL ){
            readAVPackage->setPlay();
        }
        setPlay();
    } else {
        if(readAVPackage != NULL ){
            readAVPackage->setPause();
        }
        setPause();
    }
    return 1;
}

//添加声音 adpcm_swf
void VideoDub::addVoice(char *pcm, int size) {
    if (readEnd || isExit) {
        return;
    }
    memcpy(audioBuffer , pcm ,size );
    outAFrame->data[0] = (uint8_t *) audioBuffer;
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
        LOGE("write  video %d , audio %d ", encodeFrameQue.size(), audioQue.size());
        if (audioQue.size() <= 0 || encodeFrameQue.size() <= 0) {
            if (readEnd) {
                break;
            }
            threadSleep(2);
            continue;
        }
        AVPacket *aPkt = audioQue.front();
        AVFrame *vframe = encodeFrameQue.front();
        if (av_compare_ts(apts, audioOutputStream->time_base, vpts, videoOutputStream->time_base) <
            0) {
            LOGE(" WRITE AAAAAAAAAAAAAAAAAAAAA  ");
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
            LOGE(" WRITE VVVVVVVVVVVVVVVVVVVVVVV  ");
            AVPacket *vPkt = encodeFrame(vframe, vCtxE);
            av_frame_free(&vframe);
            encodeFrameQue.pop();
            if (vPkt == NULL) {
                continue;
            }
            vPkt->stream_index = videoOutputStreamIndex;
            av_packet_rescale_ts(vPkt, inputVideoStream->time_base, videoOutputStream->time_base);
            vpts = vPkt->pts;
            result = av_interleaved_write_frame(afc_output, vPkt);
            if (result < 0) {
                LOGE(" video av_interleaved_write_frame faild ! %s ", av_err2str(result));
            }
            av_packet_free(&vPkt);
        }
    }
    writeTrail(afc_output);
    LOGE("-------------------------------- ALL END --------------------------------");
}

//这是被通知的方法
void VideoDub::update(MyData *mydata) {

    if (mydata == NULL) {
        readEnd = true;
//        LOGE(" READ END ....... ");
        return;
    }
    if (mydata->isAudio) {
        delete mydata;
        return;
    }
    while (!isExit) {
        if (frameQue.size() < maxFrameSize) {
            AVPacket *pkt = mydata->pkt;
            if (pkt == NULL) {
                LOGE("DELETE PKT == NULL ");
                delete mydata;
                return;
            }
            AVFrame *vframe = decodePacket(vCtxD, pkt);
            delete mydata;
            if (vframe != NULL) {
                if (sws != NULL) {
                    //需要转换分辨率
                    AVFrame *outVframe = av_frame_alloc();
                    outVframe->width = width;
                    outVframe->height = height;
                    outVframe->format = AV_PIX_FMT_YUV420P;
                    av_frame_get_buffer(outVframe, 0);
                    sws_scale(sws, (const uint8_t *const *) vframe->data, vframe->linesize,
                              0, vframe->height, outVframe->data, outVframe->linesize);
                    outVframe->pts = vframe->pts;
                    frameQue.push(outVframe);
                    av_frame_free(&vframe);
                } else {
                    frameQue.push(vframe);
                }
            }
            break;
        } else {
            threadSleep(2);
        };
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
    if(yuvPlayer != NULL ){
        delete yuvPlayer;
    }
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
    if(audioBuffer != NULL ){
        free(audioBuffer);
    }
    destroySwsContext();
    apts = 0;
    vpts = 0;
    LOGE(" DESTROY OUTPUT !");
}

void VideoDub::destroyOther() {
    while (!audioQue.empty()) {
        AVPacket *pkt = audioQue.front();
        if (pkt != NULL) {
            av_packet_free(&pkt);
        }
        audioQue.pop();
    }
    while (!encodeFrameQue.empty()) {
        AVFrame *frame = encodeFrameQue.front();
        if (frame != NULL) {
            av_frame_free(&frame);
        }
        encodeFrameQue.pop();
    }
    while (!frameQue.empty()) {
        AVFrame *frame = frameQue.front();
        if (frame != NULL) {
            av_frame_free(&frame);
        }
        frameQue.pop();
    }
}

VideoDub::~VideoDub() {
    //数据输入源最先停掉
    if(readAVPackage != NULL ){
        readAVPackage->stop();
        readAVPackage->join();
        delete  readAVPackage;
    }
    this->stop();
    while (!decodeEnd) {
        LOGE(" wait for end!");
        threadSleep(1);
    }
    this->join();
    destroyOther();
    destroyInput();
    destroyOutput();
    LOGE(" DESTROY SUCCESS !!!!");
}

