//
// Created by Administrator on 2018/11/20/020.
//
/**
 * https://blog.csdn.net/leixiaohua1020/article/details/25346147 这个是学习编码成jpg的
 * https://blog.csdn.net/leixiaohua1020/article/details/13506099 bitmap -》yuv
 * https://blog.csdn.net/baidu_32237719/article/details/77870408 yuv-》bitmap
 */



#include "CurrentTimeBitmap.h"


CurrentTimeBitmap::CurrentTimeBitmap(const char *path, int outWidth, int outHeight) {
    this->outWidth = outWidth;
    this->outHeight = outHeight;
    int len = strlen(path);
    len++;
    this->path = (char *) malloc(len);
    strcpy(this->path, path);
    outPix = AV_PIX_FMT_BGR24;
    int result = initFFmpeg(path);
    if (result < 0) {
        LOGE(" init ffmepg faild !");
        return;
    }
}


AVFrame *CurrentTimeBitmap::deocdePacket(AVPacket *packet) {

    int result = avcodec_send_packet(vc, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(vc, frame);
        if (result < 0) {
            LOGE(" avcodec_receive_frame  faild %s ", av_err2str(result));
            av_frame_free(&frame);
            return NULL;
        }
        return frame;
    }
    av_frame_free(&frame);
    return NULL;
}

int CurrentTimeBitmap::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outWidth, outHeight,
                         outPix, SWS_BICUBIC, NULL, NULL, NULL);

    if (sws == NULL) {
        return -1;
    }
    return 1;
}



void CurrentTimeBitmap::getCurrentBitmapKeyFrame(float time , uint8_t *bufferResult){
    int result = 0;
    result = av_seek_frame(afc, -1,
                           (time / 1000) * AV_TIME_BASE * afc->start_time,
                           AVSEEK_FLAG_BACKWARD);
//    int64_t pts = 0;
    while (true) {
        AVPacket *packet = av_packet_alloc();
        result = av_read_frame(afc, packet);
        if (result < 0) {
            LOGE(" ********************** READ FRAME FAILD *********************");
            av_packet_free(&packet);
            break;
        }
        if (packet->stream_index == video_index) {
            AVFrame *frame = deocdePacket(packet);
            if (frame != NULL) {
//                pts = (int64_t) (frame->pts * av_q2d(afc->streams[video_index]->time_base) *
//                                 1000);
//                if ((pts >= time * 1000)) {
                    sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                              0, frame->height, outRgbdata, outRgbLineSize);
                    LOGE("------------------------------------ FIND FRAME ------------------------- " );
                    int size = 0;
                    uint8_t *bmp = yuv2Bmp(outRgbdata[0], outWidth, outHeight, &size);
                    if (bmp != NULL) {
                        av_packet_free(&packet);
                        av_frame_free(&frame);
                        memcpy(bufferResult , bmp , size);
                        free(bmp);
                        return ;
                    }
//                }
                av_frame_free(&frame);
            }
        }
        av_packet_free(&packet);
    }
};


//这个会到指定时间地方
void CurrentTimeBitmap::getCurrentBitmap(float time , uint8_t *bufferResult) {
    int result = 0;
    result = av_seek_frame(afc, -1,
                           (time / 1000) * AV_TIME_BASE * afc->start_time,
                           AVSEEK_FLAG_BACKWARD);
    int64_t pts = 0;
    while (true) {
        AVPacket *packet = av_packet_alloc();
        result = av_read_frame(afc, packet);
        if (result < 0) {
            LOGE(" ********************** READ FRAME FAILD *********************");
            av_packet_free(&packet);
            break;
        }
        if (packet->stream_index == video_index) {
            AVFrame *frame = deocdePacket(packet);
            if (frame != NULL) {
                pts = (int64_t) (frame->pts * av_q2d(afc->streams[video_index]->time_base) *
                                 1000);
                if ((pts >= time * 1000)) {
                    sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                              0, frame->height, outRgbdata, outRgbLineSize);
                    LOGE("------------------------------------ FIND FRAME ------------------------- " );
                    int size = 0;
                    uint8_t *bmp = yuv2Bmp(outRgbdata[0], outWidth, outHeight, &size);
                    if (bmp != NULL) {
                        av_packet_free(&packet);
                        av_frame_free(&frame);
                        memcpy(bufferResult , bmp , size);
                        free(bmp);
                        return ;
                    }
                }
                av_frame_free(&frame);
            }
        }
        av_packet_free(&packet);
    }
}


/**
 * RGB数据->BMP
 * @param rgb24_buffer
 * @param width
 * @param height
 * @param index
 * 传入是 AV_PIX_FMT_BGR24 格式的数据
 * @return
 */
uint8_t *CurrentTimeBitmap::yuv2Bmp(uint8_t *bgr24_buffer, int width, int height, int *bmpSize) {
    BmpHead m_BMPHeader = {0};
    InfoHead m_BMPInfoHeader = {0};
    char bfType[2] = {'B', 'M'};
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
//    LOGE(" bmp header size %d ", header_size);

    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;
    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 24;
    m_BMPInfoHeader.realSize = 3 * width * height;


    /**
     * 如果传递进来的是 AV_PIX_FMT_RGB24 需要转换成 bgr 格式
     */
//    for (j = 0; j < height; j++) {
//        for (i = 0; i < width; i++) {
//            char temp = rgb24_buffer[(j * width + i) * 3 + 2];
//            rgb24_buffer[(j * width + i) * 3 + 2] = rgb24_buffer[(j * width + i) * 3 + 0];
//            rgb24_buffer[(j * width + i) * 3 + 0] = temp;
//        }
//    }

    *bmpSize = sizeof(bfType) + sizeof(m_BMPHeader) + sizeof(m_BMPInfoHeader) + width * height * 3;

    uint8_t *bmp = (uint8_t *) malloc(*bmpSize);
    memcpy(bmp, bfType, sizeof(bfType));
    memcpy(bmp + sizeof(bfType), &m_BMPHeader, sizeof(m_BMPHeader));
    memcpy(bmp + sizeof(bfType) + sizeof(m_BMPHeader), &m_BMPInfoHeader, sizeof(m_BMPInfoHeader));
    memcpy(bmp + sizeof(bfType) + sizeof(m_BMPHeader) + sizeof(m_BMPInfoHeader), bgr24_buffer,
           width * height * 3);
    return bmp;
}

/**
 * RGB帧->BMP
 * 保存BMP文件的函数
 * 还未测试
 * @param pFrameRGB
 * @param width
 * @param height
 * @param index
 * @return
 */

uint8_t *CurrentTimeBitmap::yuv2Bmp(AVFrame *pFrameRGB, int width, int height, int *bmpSize) {
    BmpHead m_BMPHeader = {0};
    InfoHead m_BMPInfoHeader = {0};
    char bfType[2] = {'B', 'M'};
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
    LOGE(" header size %d ", header_size);
    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;
    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 24;
    m_BMPInfoHeader.realSize = 3 * width * height;
    *bmpSize = sizeof(bfType) + sizeof(m_BMPHeader) + sizeof(m_BMPInfoHeader) + width * height * 3;
    LOGE(" size %d ", *bmpSize);
    uint8_t *bmp = (uint8_t *) malloc(*bmpSize);
    FILE *fp_bmp = NULL;
    if ((fp_bmp = fopen("sdcard/FFmpeg/frame.bmp", "wb")) == NULL) {
        LOGE("Error: Cannot open output BMP file.\n");
        return NULL;
    }

//    fwrite(bfType, 1, sizeof(bfType), fp_bmp);
//    fwrite(&m_BMPHeader, 1, sizeof(m_BMPHeader), fp_bmp);
//    fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader), fp_bmp);
//    fwrite(pFrameRGB->data[0], width * height * 3, 1, fp_bmp);
//    fclose(fp_bmp);
    memcpy(bmp , bfType , sizeof(bfType));
    memcpy(bmp , &m_BMPHeader , sizeof(m_BMPHeader));
    memcpy(bmp , &m_BMPInfoHeader , sizeof(m_BMPInfoHeader));
    memcpy(bmp , pFrameRGB->data[0] , width * height * 3);
    return bmp;

}


int CurrentTimeBitmap::initFFmpeg(const char *path) {

    int result = 0;
    av_register_all();
#ifdef DEBUG
    av_log_set_callback(custom_log);
#endif
    LOGE(" play path %s ", path);
    afc = NULL;
    result = avformat_open_input(&afc, path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }
    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d ,pix format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            initSwsContext(avStream->codecpar->width, avStream->codecpar->height,
                           avStream->codecpar->format);
            if (!videoCode) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }

    if (videoCode == NULL) {
        return RESULT_FAILD;
    }

    LOGE(" video code name %s  ", videoCode->name);


    vc = avcodec_alloc_context3(videoCode);
    if (!vc) {
        LOGE("vc AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc, afc->streams[video_index]->codecpar);

    vc->thread_count = 4;

    result = avcodec_open2(vc, NULL, NULL);
    if (result != 0) {
        LOGE("vc avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    result = av_image_alloc(outRgbdata, outRgbLineSize, outWidth, outHeight, outPix, 1);
    if (result < 0) {
        LOGE("Could not allocate destination image\n");
        return -1;
    }
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}


CurrentTimeBitmap::~CurrentTimeBitmap() {
    if (vc != NULL) {
        avcodec_close(vc);
        vc = NULL;
    }
    if (afc != NULL) {
        avformat_close_input(&afc);
        afc = NULL;
    }
    if (sws != NULL) {
        sws_freeContext(sws);
        sws = NULL;
    }
    if(outRgbdata != NULL){
        av_freep(&outRgbdata[0]);
    }
    if(path != NULL){
        free(path);
    }


}