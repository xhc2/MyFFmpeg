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
    outPix = AV_PIX_FMT_RGB24;
    int result = initFFmpeg(path);
    if (result < 0) {
        LOGE(" init ffmepg faild !");
        return;
    }
    testYuv2Bitmap();
}

void CurrentTimeBitmap::testYuv2Bitmap(){
    LOGE(" ----------------------testYuv2Bitmap--------------------------");

    int src_w = 640 ;
    int src_h = 360 ;
    AVPixelFormat src_pixfmt = AV_PIX_FMT_YUV420P;
    int dst_w = 300;
    int dst_h = 300;
    AVPixelFormat dst_pixfmt = AV_PIX_FMT_RGB24;

    uint8_t *src_data[4];
    int src_linesize[4];

    uint8_t *dst_data[4];
    int dst_linesize[4];

    int rescale_method = SWS_BICUBIC;
    struct SwsContext *img_convert_ctx;
    uint8_t *temp_buffer = (uint8_t *) malloc(src_w * src_h * 3 / 2);

    int ret = 0;
    ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);
    if (ret < 0) {
        LOGE("Could not allocate source image\n");
        return ;
    }
    ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
    if (ret < 0) {
        LOGE("Could not allocate destination image\n");
        return  ;
    }

    LOGE("testYuv2Bitmap dst_linesize %d  " , dst_linesize[0]);

    img_convert_ctx = sws_alloc_context();

    av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
    av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
    av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
    //'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
    av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
    av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
    av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
    av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
    av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
    sws_init_context(img_convert_ctx, NULL, NULL);

    FILE *yuvF = fopen("sdcard/FFmpeg/oneframe.yuv" , "rb");
    fread(temp_buffer, 1, src_w * src_h * 3 / 2, yuvF);
    memcpy(src_data[0], temp_buffer, src_w * src_h);                    //Y
    memcpy(src_data[1], temp_buffer + src_w * src_h, src_w * src_h / 4);      //U
    memcpy(src_data[2], temp_buffer + src_w * src_h * 5 / 4, src_w * src_h / 4);  //V
    sws_scale(img_convert_ctx, (const uint8_t *const *) src_data, src_linesize, 0, src_h,
              dst_data, dst_linesize);
    LOGE(" SRC_LINESIZE %d , dst_lineSize %d " ,src_linesize[0] , dst_linesize[0] );
    int size = 0 ;
    FILE *yuvRgb = fopen("sdcard/FFmpeg/oneframe.rgb" , "wb+");
    fwrite(dst_data[0] , 1, dst_w* dst_h * 3 ,yuvRgb );
    fclose(yuvRgb);
//    yuv2Bmp(dst_data[0], dst_w, dst_h , &size);
    LOGE(" ----------------------testYuv2Bitmap----end----------------------");
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
//    sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outWidth, outHeight,
//                         outPix, SWS_BICUBIC, NULL, NULL, NULL);
    sws = sws_alloc_context();
    LOGE("testYuv2Bitmap  inwidth %d , inHeight %d , outWidth %d , outHeight %d " , inWidth , inHeight , outWidth , outHeight);
    av_opt_set_int(sws, "srcw", inWidth, 0);
    av_opt_set_int(sws, "srch", inHeight, 0);
    av_opt_set_int(sws, "src_format", inpixFmt, 0);
    //'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
    av_opt_set_int(sws, "src_range", 1, 0);
    av_opt_set_int(sws, "dstw", outWidth, 0);
    av_opt_set_int(sws, "dsth", outHeight, 0);
    av_opt_set_int(sws, "dst_format", outPix, 0);
    av_opt_set_int(sws, "dst_range", 1, 0);
    sws_init_context(sws, NULL, NULL);

    if (sws == NULL) {
        return -1;
    }
    return 1;
}

char* CurrentTimeBitmap::getCurrentBitmap(float time) {
    int result = av_seek_frame(afc, -1,
                               (time / 1000) * AV_TIME_BASE * afc->start_time,
                               AVSEEK_FLAG_BACKWARD);
    int64_t pts = 0;
    FILE *out = fopen("sdcard/FFmpeg/test.rgb" , "wb+");
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
                    outVFrame->linesize[0] = 900;
                    sws_scale(sws, (const uint8_t *const *) frame->data, frame->linesize,
                              0, frame->height, outVFrame->data, outVFrame->linesize);
                    LOGE("------------------------------------ FIND FRAME  linesize %d , height %d " , outVFrame->linesize[0] , outVFrame->height);

                    fwrite(outVFrame->data[0]  ,1 ,outVFrame->width *outVFrame->height * 3 , out );
                    fclose(out);
                    int size = 0;
                    uint8_t * bmp = yuv2Bmp(outVFrame->data[0] , outVFrame->width , outVFrame->height , &size );

                    LOGE(" BMP SIZE %d " , size);
                    if(bmp != NULL){
//                        fwrite(bmp , 1 ,size ,out );
                    }

                    av_frame_free(&frame);
                    break;
                }
            }

            av_packet_free(&packet);
        }
    }

    return NULL;
}


/**
 * RGB数据->BMP
 * @param rgb24_buffer
 * @param width
 * @param height
 * @param index
 * 传入是 AV_PIX_FMT_RGB24 格式的数据
 * @return
 */
uint8_t * CurrentTimeBitmap::yuv2Bmp(uint8_t *rgb24_buffer, int width, int height , int *bmpSize) {
    int i = 0, j = 0;
    BmpHead m_BMPHeader = {0};
    InfoHead m_BMPInfoHeader = {0};
    char bfType[2] = {'B', 'M'};
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
    LOGE(" bmp header size %d " , header_size);
    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;

    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 24;
    m_BMPInfoHeader.realSize = 3 * width * height;

    //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2
    //It saves pixel data in Little Endian
    //So we change 'R' and 'B'
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            char temp = rgb24_buffer[(j * width + i) * 3 + 2];
            rgb24_buffer[(j * width + i) * 3 + 2] = rgb24_buffer[(j * width + i) * 3 + 0];
            rgb24_buffer[(j * width + i) * 3 + 0] = temp;
        }
    }

    *bmpSize = sizeof(bfType) + sizeof(m_BMPHeader) +sizeof(m_BMPInfoHeader) + width * height * 3;


    uint8_t *bmp = (uint8_t *)malloc(*bmpSize);
//    memcpy(bmp , bfType , sizeof(bfType));
//    memcpy(bmp + sizeof(bfType), &m_BMPHeader , sizeof(m_BMPHeader));
//    memcpy(bmp + sizeof(bfType) + sizeof(m_BMPHeader), &m_BMPInfoHeader , sizeof(m_BMPInfoHeader));
//    memcpy(bmp+ sizeof(bfType) + sizeof(m_BMPHeader)+sizeof(m_BMPInfoHeader) , rgb24_buffer , width * height * 3);
    FILE *fp_bmp = fopen("sdcard/FFmpeg/bmp.bmp", "wb+");
    fwrite(bfType, 1, sizeof(bfType), fp_bmp);
    fwrite(&m_BMPHeader, 1, sizeof(m_BMPHeader), fp_bmp);
    fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader), fp_bmp);
    fwrite(rgb24_buffer, 3 * width * height, 1, fp_bmp);
    fclose(fp_bmp);
    //rgb24_buffer 是一张 bmp 了
    return bmp;
}

/**
 * RGB帧->BMP
 * 保存BMP文件的函数
 * @param pFrameRGB
 * @param width
 * @param height
 * @param index
 * @return
 */

uint8_t * CurrentTimeBitmap::yuv2Bmp(AVFrame *pFrameRGB, int width, int height , int *bmpSize ) {
    BmpHead m_BMPHeader = {0};
    InfoHead m_BMPInfoHeader = {0};
    char bfType[2] = {'B', 'M'};
    int header_size = sizeof(bfType) + sizeof(BmpHead) + sizeof(InfoHead);
    LOGE(" header size %d " , header_size );
    m_BMPHeader.imageSize = 3 * width * height + header_size;
    m_BMPHeader.startPosition = header_size;
    m_BMPInfoHeader.Length = sizeof(InfoHead);
    m_BMPInfoHeader.width = width;
    //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
    m_BMPInfoHeader.height = -height;
    m_BMPInfoHeader.colorPlane = 1;
    m_BMPInfoHeader.bitColor = 24;
    m_BMPInfoHeader.realSize = 3 * width * height;
    *bmpSize = sizeof(bfType) + sizeof(m_BMPHeader) +sizeof(m_BMPInfoHeader) + width * height * 3;
    LOGE(" size %d " , *bmpSize);
    uint8_t *bmp = (uint8_t *)malloc(*bmpSize);
    FILE *fp_bmp = NULL;
    if ((fp_bmp = fopen("sdcard/FFmpeg/frame.bmp", "wb")) == NULL) {
        LOGE("Error: Cannot open output BMP file.\n");
        return NULL;
    }

    fwrite(bfType, 1, sizeof(bfType), fp_bmp);
    fwrite(&m_BMPHeader, 1, sizeof(m_BMPHeader), fp_bmp);
    fwrite(&m_BMPInfoHeader, 1, sizeof(m_BMPInfoHeader), fp_bmp);
    fwrite(pFrameRGB->data[0], width * height * 3, 1, fp_bmp);
    fclose(fp_bmp);
//    memcpy(bmp , bfType , sizeof(bfType));
//    memcpy(bmp , &m_BMPHeader , sizeof(m_BMPHeader));
//    memcpy(bmp , &m_BMPInfoHeader , sizeof(m_BMPInfoHeader));
//    memcpy(bmp , pFrameRGB->data[0] , width * height * 3);
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

    outVFrame = av_frame_alloc();
    outVFrame->width = outWidth;
    outVFrame->height = outHeight;
    outVFrame->format = outPix;
    result = av_frame_get_buffer(outVFrame, 0);
    if (result < 0) {
        LOGE(" av_frame_get_buffer FAILD ! ");
        return -1;
    }
    result = av_frame_make_writable(outVFrame);
    if (result < 0) {
        LOGE(" av_frame_make_writable FAILD ! ");
        return -1;
    }
    LOGE(" outVFrame LINE SIZE %d " , outVFrame->linesize[0]);
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}


CurrentTimeBitmap::~CurrentTimeBitmap() {

}