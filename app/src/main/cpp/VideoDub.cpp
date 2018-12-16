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

void VideoDub::allocAudioFifo(AVSampleFormat sample_fmt, int channels, int nb_samples) {
    if (audioFifo == NULL) {
        audioFifo = av_audio_fifo_alloc(sample_fmt, channels, nb_samples);
    }
};


int VideoDub::buildOutput(const char *outputPath) {
    int result = 0;
    outChannel = 1;
    outChannelLayout = AV_CH_LAYOUT_MONO;
    sampleFormat =  AV_SAMPLE_FMT_S16 ;//AV_SAMPLE_FMT_FLTP;
    sampleRate = 44100;

    afc_output = NULL;
    apts = 0 ;
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
    LOGE("  aCtxE SIZE %d " , aCtxE->frame_size);
    if (result < 0) {
        LOGE(" addOutputAudioStream FAILD !");
        return -1;
    }
    audioOutputStreamIndex = result;
    audioOutputStream = afc_output->streams[result];
    //这是从录音过来的
//    result = initSwrContext(1, AV_SAMPLE_FMT_S16, 44100);
//    if (result < 0) {
//        LOGE(" initSwrContext FAILD ! ");
//        return -1;
//    }



    result = writeOutoutHeader(afc_output, outputPath);
    if (result < 0) {
        LOGE(" writeOutoutHeader faild ! ");
        return -1;
    }
    nbSample = aCtxE->frame_size;
    audioFifoBufferSample = nbSample * 3;
    src_data = (uint8_t **)malloc(8 * sizeof(uint8_t *));
    result = av_samples_alloc(src_data ,&src_linesize ,  outChannel , nbSample , sampleFormat ,0);
    if(result < 0){
        LOGE(" av_samples_alloc faild ! ");
        return -1;
    }
    audioOutBuffer = (uint8_t *) malloc(av_get_bytes_per_sample(sampleFormat) * nbSample);
    outAFrame = av_frame_alloc();
    outAFrame->sample_rate = sampleRate ;
    outAFrame->format = sampleFormat;
    outAFrame->channels = outChannel;
    outAFrame->channel_layout = outChannelLayout ;
    outAFrame->nb_samples = nbSample;
    result = av_frame_make_writable(outAFrame);
    if(result < 0){
        LOGE(" av_frame_make_writable %s  " , av_err2str(result));
    }
    aCalDuration =  AV_TIME_BASE  / sampleRate;
    LOGE(" BUILD OUTPUT SUCCESS ! videoindex %d , audioindex %d " , videoOutputStreamIndex , audioOutputStreamIndex);
//    allocAudioFifo(sampleFormat , outChannel , audioFifoBufferSample);

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
//            writeTrail(afc_output);
            LOGE(" read end !");
            break;
        }
        if (pkt->stream_index == videoStreamIndex) {
            AVFrame *vframe = decodePacket(vCtxD, pkt);
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
                AVPacket *vPkt = encodeFrame(vframe , vCtxE);
                av_frame_free(&vframe);
                if(vPkt != NULL){
//                    av_packet_rescale_ts(vPkt , inputVideoStream->time_base , videoOutputStream->time_base );
//                    av_write_frame(afc_output ,vPkt );
                    videoQue.push(vPkt);
                }
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
//添加声音 adpcm_swf
void VideoDub::addVoice(char *pcm, int size) {
    if(readEnd){
        return ;
    }
//    int result;
//    if(pcmF == NULL){
////        pcmF = fopen("sdcard/FFmpeg/pcm.pcm" , "wb+");
////    }
    memcpy(src_data[0] , pcm , size);
    outAFrame->data[0] = src_data[0] ;
    outAFrame->linesize[0] = size  ;
    audioCount += size / av_get_bytes_per_sample(sampleFormat) ;
    outAFrame->pts = audioCount * aCalDuration ;
//    fwrite(outAFrame->data[0]  ,1 , outAFrame->linesize[0] , pcmF) ;
    AVPacket *pkt = encodeFrame(outAFrame , aCtxE);
    if(pkt != NULL){
//        pkt->stream_index = audioOutputStreamIndex;
//        av_packet_rescale_ts(pkt , timeBaseFFmpeg , audioOutputStream->time_base );
//        LOGE(" PKT SIZE %d , pts %lld " , pkt->size , pkt->pts);
//          result = av_write_frame(afc_output ,pkt );
//        if(result < 0){
//            LOGE(" WRITE AUDIO FAILD ! %s" , av_err2str(result));
//        }
        audioQue.push(pkt);
    }


//    LOGE("outNbSample %d ,nbSample %d " , size /  av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) , nbSample);

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
//            LOGE(" write aaaaaaaaaaaaaa " );
            aPkt->stream_index = audioOutputStreamIndex ;
            av_packet_rescale_ts(aPkt , timeBaseFFmpeg , audioOutputStream->time_base );
            apts = aPkt->pts;
            result = av_interleaved_write_frame(afc_output, aPkt);
            if (result < 0) {
                LOGE(" audio av_interleaved_write_frame  faild ! %s " , av_err2str(result) );
            }
            av_packet_free(&aPkt);
            audioQue.pop();
        } else {
//            LOGE(" write vvvvvvvvvvvvvvv " );
            vPkt->stream_index = videoOutputStreamIndex ;
            av_packet_rescale_ts(vPkt , inputVideoStream->time_base , videoOutputStream->time_base );
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

