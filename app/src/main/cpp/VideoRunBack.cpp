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

const char *tempYuv = "sdcard/FFmpeg/temp.yuv";

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

    int result = initInput();
    if (result < 0) {
        LOGE(" initInput faild ! ");
        return;
    }
    result = buildOutput();
    if (result < 0) {
        LOGE(" initOutput faild ! ");
        return;
    }
    yuvSize = inWidth * inHeight * 3 / 2;
    ySize = inWidth * inHeight ;
    LOGE(" YUV SIZE %d " , yuvSize);
    readBuffer = (char *) malloc(yuvSize);
}

void VideoRunBack::initValue() {
    afc_output = NULL;
    afc_input = NULL;
    audioIndexInput = -1;
    videoIndexInput = -1;
    videoIndexOutput = -1;
    audioIndexOutput = -1;
    gopCount = 0;
    frameDuration = 0;
    encodeFrameCount = 0;
    vpts = 0;
    apts = 0;
    frameDuration = AV_TIME_BASE / getVideoOutFrameRate();
}


int VideoRunBack::initInput() {
    int result;

    result = open_input_file(inputPath, &afc_input);
    if (result < 0 || afc_input == NULL) {
        LOGE(" open_input_file faild ! ");
        return -1;
    }
    result = getVideoDecodeContext(afc_input, &vCtxD);
    if (result < 0 || vCtxD == NULL) {
        LOGE(" getVideoDecodeContext faild ! ");
        return -1;
    }
    videoIndexInput = result;

    audioIndexInput = getAudioStreamIndex(afc_input);
    if (audioStreamIndex == -1) {
        LOGE(" find audio Stream faild ! ");
        return -1;
    }
    inWidth = afc_input->streams[videoIndexInput]->codecpar->width;
    inHeight = afc_input->streams[videoIndexInput]->codecpar->height;

    int64_t duration = (int64_t) (afc_input->duration * av_q2d(timeBaseFFmpeg));
    if (duration > 120) {
        LOGE(" duration > 120 !");
        return -1;
    }

    LOGE(" inputduration %lld ", afc_input->duration);
    return 1;
}

int VideoRunBack::buildOutput() {
    int result;
    afc_output = NULL;
    result = initOutput(outPath, &afc_output);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    result = addOutputVideoStream(afc_output, &vCtxE,
                                  *afc_input->streams[videoIndexInput]->codecpar);
    if (result < 0) {
        LOGE(" addVideoOutputStream FAILD ! ");
        return -1;
    }
    videoOutputStreamIndex = result;
    result = addOutputAudioStream(afc_output , NULL , *afc_input->streams[audioIndexInput]->codecpar);
    if (result < 0) {
        LOGE(" addAudioOutputStream ");
        return -1;
    }
    audioOutputStreamIndex = result;
    writeOutoutHeader(afc_output, this->outPath);
    AVCodecParameters *codecpar = afc_input->streams[videoIndexInput]->codecpar;
    outFrame = av_frame_alloc();
    outFrame->width = codecpar->width;
    outFrame->height = codecpar->height;
    outFrame->format = codecpar->format;
//    av_frame_get_buffer(outFrame, 0);
    LOGE(" LINE %d , %d , %d " , outFrame->linesize[0] , outFrame->linesize[1] , outFrame->linesize[2]);
    LOGE("format  %d ， width %d , height %d ", codecpar->format , outFrame->width , outFrame->height);
//    av_frame_get_buffer 会产生内存泄露。后面好好检查这块
    return 1;
}




void VideoRunBack::run() {
    while(!isExit){

        pthread_mutex_lock(&mutex_pthread);
        LOGE(" videoQue %d ， audioQue %d  ", queVideo.size(), audioStack.size());
        if (queVideo.size() <= 0 || audioStack.size() <= 0) {
            pthread_mutex_unlock(&mutex_pthread);
//            if (readEnd) {
//                break;
//            }
            continue;
        }
        AVPacket *aPkt = audioStack.top();
        AVPacket *vPkt = queVideo.front();
        if(av_compare_ts(apts, afc_output->streams[audioIndexOutput]->time_base, vpts, afc_output->streams[videoIndexOutput]->time_base) < 0){
            //write audio
//            aPkt->size
        }
        else{
            //write video

        }
    }
}

//int VideoRunBack::test(){
////    av_register_all();
//    const char * path = "sdcard/FFmpeg/testback.yuv";
//    FILE *fileTest = fopen(path , "r");
//    int width = 1280;
//    int height = 720;
//    int ySize = width * height;
//    int yuvSize = ySize * 3 / 2 ;
//    char* tempRead = (char *)malloc(yuvSize);
//    int result;
////    afc_output = NULL;
//    result = initOutput(outPath, &afc_output);
//    if (result < 0 || afc_output == NULL) {
//        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
//        return -1;
//    }
//    AVCodecParameters *codecpar = new AVCodecParameters();
//    codecpar->width = width ;
//    codecpar->height = height ;
//    codecpar->format = AV_PIX_FMT_YUV420P ;
//    codecpar->codec_type = AVMEDIA_TYPE_VIDEO ;
//    result = addOutputVideoStream(afc_output, &vCtxE , *codecpar);
//    if (result < 0) {
//        LOGE(" addVideoOutputStream FAILD ! ");
//        return -1;
//    }
//    videoOutputStreamIndex = result;
//    LOGE(" addOutputVideoStream %s " ,this->outPath );
//    writeOutoutHeader(afc_output, this->outPath);
//    LOGE(" writeOutoutHeader ");
//
//    outFrame = av_frame_alloc();
//    outFrame->width = width;
//    outFrame->height = height;
//    outFrame->format = AV_PIX_FMT_YUV420P;
//    int len = 0;
//    while(true){
//        len = fread(tempRead , 1 , yuvSize , fileTest);
//        if(len != yuvSize ){
//            LOGE(" READ END !");
//            break;
//        }
//        outFrame->data[0] = (uint8_t *) tempRead;
//        outFrame->data[1] = (uint8_t *) (tempRead + yuvSize);
//        outFrame->data[2] = (uint8_t *) (tempRead + yuvSize + yuvSize / 4);
//
//        outFrame->linesize[0] = width;
//        outFrame->linesize[1] = width / 2;
//        outFrame->linesize[2] = width / 2;
//        outFrame->pts = encodeFrameCount * frameDuration;
//        encodeFrameCount++;
//        LOGE(" encodeFrameCount %d" , encodeFrameCount );
//        AVPacket *pkt = encodeFrame(outFrame , vCtxE);
//        if(pkt != NULL){
//            LOGE(" ENCODE FRAME ");
//            av_packet_rescale_ts(pkt, timeBaseFFmpeg,
//                                 afc_output->streams[videoOutputStreamIndex]->time_base);
//            av_write_frame(afc_output , pkt);
//            av_packet_free(&pkt);
//        }
//
//    }
//    writeTrail(afc_output);
//
//    return 1;
//}


int VideoRunBack::startBackParse() {
    LOGE(" -------------------start------------------------ ");
    int result = 0;
//    this->start();
    fCache = fopen(tempYuv, "wb+");
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
            if (pkt->flags & AV_PKT_FLAG_KEY) {
                keyFrameQue.push_back(pkt->pts);
                LOGE(" KEY FRAME %lld ", pkt->pts);
            }
            frameCount++;
            videoStreamDuration = pkt->pts;
        }
    }
    LOGE(" QUE SIZE %d ", keyFrameQue.size());

    int nowKeyFramePosition = keyFrameQue.size() - 1;
    gopCount++;
    result = av_seek_frame(afc_input, videoIndexInput, keyFrameQue.at(nowKeyFramePosition),
                           AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE(" av_seek_frame %s ", av_err2str(result));
        return -1;
    }
    while (true) {
        result = av_read_frame(afc_input, pkt);
        if (result < 0) {
            gopCount++;
            clearCode(fCache);
            //这里也需要逆序读取
            reverseFile();
            nowKeyFramePosition--;
            if (nowKeyFramePosition > 0) {
                result = av_seek_frame(afc_input, videoIndexInput,
                                       keyFrameQue.at(nowKeyFramePosition), AVSEEK_FLAG_BACKWARD);
                if (result < 0) {
                    LOGE(" SEEK FAILD MAYBE FINISH ");
                    break;
                }
                continue;
            }
            break;
        }

        if (pkt->stream_index == audioIndexInput) {
            audioStack.push(pkt);
            LOGE(" AUDIO PKT %d " , pkt->size );
            continue;
        }
        if (pkt->stream_index == videoIndexInput) {

            if (((nowKeyFramePosition + 1) >= keyFrameQue.size() &&
                 pkt->pts > videoStreamDuration) ||
                (nowKeyFramePosition + 1) < keyFrameQue.size() &&
                pkt->pts > keyFrameQue.at(nowKeyFramePosition + 1)) {
                //完成了一个gop
                clearCode(fCache);
                nowKeyFramePosition--;
                if (nowKeyFramePosition >= 0) {
                    gopCount++;
                    result = av_seek_frame(afc_input, videoIndexInput,
                                           keyFrameQue.at(nowKeyFramePosition),
                                           AVSEEK_FLAG_BACKWARD);
                    if (result < 0) {
                        LOGE(" SEEK FAILD MAYBE FINISH ");
                        break;
                    }
                }
                LOGE(" NEXT GOP %d", nowKeyFramePosition);

                //开始倒序读取
                reverseFile();
                if (nowKeyFramePosition < 0) {
                    LOGE(" ALL END gopCount %d ", gopCount);
                    break;
                }
            }
            AVFrame *vFrame = decodePacket(vCtxD, pkt);
            if (vFrame != NULL) {
                writeFrame2File(vFrame, fCache);
            }
        }
    }
    writeTrail(afc_output);
    LOGE(" END write frame ");
    return 1;
}

int VideoRunBack::reverseFile() {
    fflush(fCache);
    fseek(fCache, 0, SEEK_END);
    while (true) {
        if (ftell(fCache) <= 0) {
            break;
        }
        fseek(fCache, -yuvSize, SEEK_CUR);
        fread(readBuffer, 1, yuvSize, fCache); //这里光标又往前走了yuvsize
        fseek(fCache, -yuvSize, SEEK_CUR); //把读取的光标位置放回去
//        av_frame_get_buffer(outFrame, 0);
        av_frame_make_writable(outFrame);
        outFrame->data[0] = (uint8_t *) readBuffer;
        outFrame->data[1] = (uint8_t *) (readBuffer + ySize);
        outFrame->data[2] = (uint8_t *) (readBuffer +  ySize + ySize / 4);

        outFrame->linesize[0] = inWidth;
        outFrame->linesize[1] = inWidth / 2;
        outFrame->linesize[2] = inWidth / 2;

        outFrame->pts = encodeFrameCount * frameDuration;
        encodeFrameCount++;
        AVPacket *pkt = encodeFrame(outFrame, vCtxE);
        av_frame_unref(outFrame);
        if (pkt != NULL) {
            LOGE(" WRITE PKT ");
            av_packet_rescale_ts(pkt, timeBaseFFmpeg,
                                 afc_output->streams[videoOutputStreamIndex]->time_base);
//            queVideo.push(pkt);
            av_write_frame(afc_output, pkt);
            av_packet_free(&pkt);
        }
    }
    fclose(fCache);
    fCache = fopen(tempYuv, "wb+");
    return 1;
}

void VideoRunBack::clearCode(FILE *file) {
    avcodec_flush_buffers(vCtxD);
    int result;
    do {
        AVFrame *vFrame = av_frame_alloc();
        result = avcodec_receive_frame(vCtxD, vFrame);
        if (result > 0) {
            writeFrame2File(vFrame, file);
            LOGE(" CLEAR VIDEO FLUSH SUCCESS !!!!!");
        } else {
            av_frame_free(&vFrame);
        }
    } while (result > 0);
}


void VideoRunBack::writeFrame2File(AVFrame *vFrame, FILE *file) {
    fwrite(vFrame->data[0], 1, vFrame->linesize[0] * inHeight, file);
    fwrite(vFrame->data[1], 1, vFrame->linesize[1] * inHeight / 2, file);
    fwrite(vFrame->data[2], 1, vFrame->linesize[2] * inHeight / 2, file);
    av_frame_free(&vFrame);
}


VideoRunBack::~VideoRunBack() {

}