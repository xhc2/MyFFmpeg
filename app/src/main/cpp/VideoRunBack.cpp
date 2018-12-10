/**
 * 倒放
 * 视频部分：
 * 找到一个gop然后顺序解码（实践发现是找不到的一个gop有多少帧）
 * 一个解码的gop
 * 然后写入yuv文件中，然后倒序读入中进行编码
 * 音频部分：
 * 直接写入队列中缓存队列中。
 *
 * 输出部分尽量和原文件保持一致（采样率，声道，之类的）
 *
 * seek的操作需要容器支持
 * http://bbs.chinaffmpeg.com/forum.php?mod=viewthread&tid=14
 *
 * 更换策略
 * 1.先全部遍历一遍。
 * 2.获取一共多少视频帧
 * 3.获取视频总时间
 * 4.获取关键帧的时间并放入队列
 * 5.然后seek到最后的关键帧。
 * 6.正向把yuv写入文件中
 * 7.然后逆向读取yuv文件。
 * 8.编码yuv。
 *
 */

#include <my_log.h>
#include "VideoRunBack.h"

VideoRunBack::VideoRunBack(const char *path, const char *outPath) {

    int inputLen = strlen(path);
    inputLen++;
    this->inputPath = (char *) malloc(inputLen);
    strcpy(this->inputPath, path);

    int len = strlen(outPath);
    len++;
    this->outPath = (char *) malloc(len);
    strcpy(this->outPath, outPath);
    initValue();

}

void VideoRunBack::initValue() {
    afc_output = NULL;
    afc_input = NULL;
    videoCodecD = NULL;
    audioIndexInput = -1;
    videoIndexInput = -1;
    videoIndexOutput = -1;
    audioIndexOutput = -1;
    outFrameRate = 25;
    afot = NULL;
    audioOutStream = NULL;
    videoOutStream = NULL;
    gopFrameCount = 0;
    inputDuration = 0;
    gopCount = 0;
    frameDuration = 0;
}


int VideoRunBack::initInput() {
    int result;
    result = avformat_open_input(&afc_input, inputPath, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return -1;
    }
    result = avformat_find_stream_info(afc_input, 0);
    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return -1;
    }
    for (int i = 0; i < afc_input->nb_streams; ++i) {
        AVStream *stream = afc_input->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndexInput = i;
            videoinputStream = stream;
            inWidth = stream->codecpar->width;
            inHeight = stream->codecpar->height;
            LOGE(" inwidth %d , inheight %d ", inWidth, inHeight);
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioIndexInput = i;
            audioinputStream = stream;
        }
    }

//  videoCodecD
    if (videoCodecD == NULL) {
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    LOGE(" videoCodecD %s ", videoCodecD->name);
    vCtxD = avcodec_alloc_context3(videoCodecD);
    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }
    result = avcodec_parameters_to_context(vCtxD, afc_input->streams[videoIndexInput]->codecpar);
    if (result < 0) {
        LOGE(" avcodec_parameters_to_context Faild !");
        return -1;
    }
    result = avcodec_open2(vCtxD, videoCodecD, NULL);
    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }
    int64_t duration = (int64_t) (afc_input->duration * av_q2d(timeBaseFFmpeg));
    if (duration > 60) {
        LOGE(" duration > 60 !");
        return -1;
    }

    LOGE(" inputduration %lld ", afc_input->duration);
    return 1;
}

int VideoRunBack::initOutput() {
    int result;
    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, outPath);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    result = addVideoOutputStream(inWidth, inHeight);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    result = addAudioOutputStream();
    if (result < 0) {
        LOGE(" addAudioOutputStream ");
        return -1;
    }

    return 1;
}


int VideoRunBack::addVideoOutputStream(int width, int height) {
    int result = 0;
    videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    videoIndexOutput = videoOutStream->index;
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    videoCodecE = avcodec_find_encoder(afot->video_codec);
    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return -1;
    }
    LOGE("video ENCODE NAME %s ", videoCodecE->name);
    vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }

    vCtxE->bit_rate = videoinputStream->codecpar->bit_rate;
    vCtxE->time_base = (AVRational) {1, outFrameRate};
    vCtxE->framerate = (AVRational) {outFrameRate, 1};
    vCtxE->gop_size = gopSize;
    vCtxE->max_b_frames = 1;
    vCtxE->pix_fmt = AV_PIX_FMT_YUV420P;
    vCtxE->codec_type = AVMEDIA_TYPE_VIDEO;
    vCtxE->width = width;
    vCtxE->height = height;

    result = avcodec_parameters_from_context(videoOutStream->codecpar, vCtxE);

    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }

    result = avcodec_open2(vCtxE, videoCodecE, NULL);

    if (result < 0) {
        LOGE("video Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO !");

    return 1;
}

void VideoRunBack::run() {


}

int VideoRunBack::addAudioOutputStream() {

    return 1;
}

int VideoRunBack::startBackParse() {
    LOGE(" -------------------start------------------------ ");
    int result = 0;
    const char *tempYuv = "sdcard/FFmpeg/temp.yuv";
    FILE *fCache = fopen(tempYuv, "wb+");
    FILE *testFile = fopen("sdcard/FFmpeg/test.yuv", "wb+");


    result = initInput();
    if (result < 0) {
        LOGE(" initInput faild ! ");
        return -1;
    }
    result = initOutput();
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return -1;
    }
    AVPacket *pkt = av_packet_alloc();
    //该视频的总帧数
    int frameCount = 0;
    int64_t videoStreamDuration = 0;
    int64_t videoStartTime = 0;

    while (true) {
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            break;
        }
        //获取视频流的duration

        if (pkt->stream_index == videoIndexInput) {
            if (videoStartTime == 0) {
                videoStartTime = pkt->pts;
            }
            if(pkt->flags & AV_PKT_FLAG_KEY ){
                keyFrameQue.push_back(pkt->pts);
            }
            frameCount++;
            videoStreamDuration = pkt->pts;
        }
    }
    frameDuration = videoStreamDuration / frameCount;

//    LOGE(" frameCount %d , videoStreamDuration %lld  , frameDuration %d  , startTime %lld ",
//         frameCount, videoStreamDuration, frameDuration, videoStartTime);

    int writeFrame = 0;

    int yuvSize = inWidth * inHeight * 3 / 2;
    char *readBuffer = (char *) malloc(yuvSize);
    int nowKeyFramePosition = keyFrameQue.size() - 1;
    result = av_seek_frame(afc_input, videoIndexInput,keyFrameQue.at(nowKeyFramePosition) , AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE(" av_seek_frame %s ", av_err2str(result));
        return -1;
    }
    while(true){
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            gopCount ++;
            nowKeyFramePosition -- ;
            result = av_seek_frame(afc_input, videoIndexInput,keyFrameQue.at(nowKeyFramePosition) , AVSEEK_FLAG_BACKWARD);
            if (result < 0) {
                LOGE(" SEEK FAILD MAYBE FINISH ");
                break;
            }
            LOGE("-----------------------------------  end of file -----------------------------------");
            continue;
        }
        if(((nowKeyFramePosition + 1 ) >= keyFrameQue.size() &&  pkt->pts > videoStreamDuration) ||
                (nowKeyFramePosition + 1) < keyFrameQue.size() && pkt->pts > keyFrameQue.at(nowKeyFramePosition + 1)){
            //完成了一个gop
            avcodec_flush_buffers(vCtxD);
            do{
                AVFrame *vFrame = av_frame_alloc();
                result = avcodec_receive_frame(vCtxD, vFrame);
                if(result > 0){
                    writeFrame++;
                    writeFrame2File(vFrame, fCache);
                    LOGE(" CLEAR VIDEO FLUSH SUCCESS !!!!!");
                }
                else{
                    av_frame_free(&vFrame);
                }
            }while(result > 0);

            nowKeyFramePosition -- ;
            if(nowKeyFramePosition >= 0){
                result = av_seek_frame(afc_input, videoIndexInput,keyFrameQue.at(nowKeyFramePosition) , AVSEEK_FLAG_BACKWARD);
                if (result < 0) {
                    LOGE(" SEEK FAILD MAYBE FINISH ");
                    break;
                }
            }
            fflush(fCache);
            //开始倒序读取
            fseek(fCache, 0, SEEK_END);
            while (true) {
                LOGE(" NOW FILE POSITION %ld "  , ftell(fCache));
                if(ftell(fCache) <= 0 ){
                    break;
                }
                LOGE(" SEEK CUR %d" , fseek(fCache, -yuvSize, SEEK_CUR));
                fread(readBuffer, 1, yuvSize, fCache);
                fwrite(readBuffer, 1, yuvSize, testFile);


            }
            fclose(fCache);
            fCache = fopen(tempYuv, "wb+");

            if(nowKeyFramePosition < 0){
                LOGE(" ALL END ");
                break;
            }
        }
        if (pkt->stream_index == audioIndexInput) {
            continue;
        }
        //                ftell(fCache);
        if (pkt->stream_index == videoIndexInput) {
            AVFrame *vFrame = decodePacket(vCtxD, pkt);
            if(vFrame != NULL){
                writeFrame++;
                writeFrame2File(vFrame, fCache);
            }
        }
    }

    LOGE(" END write frame %d ", writeFrame);
    return 1;
}

void VideoRunBack::writeFrame2File(AVFrame *vFrame, FILE *file) {
    fwrite(vFrame->data[0], 1, vFrame->linesize[0] * inHeight, file);
    fwrite(vFrame->data[1], 1, vFrame->linesize[1] * inHeight / 2, file);
    fwrite(vFrame->data[2], 1, vFrame->linesize[2] * inHeight / 2, file);
    av_frame_free(&vFrame);
}


VideoRunBack::~VideoRunBack() {

}