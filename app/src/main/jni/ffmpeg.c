

#include <module_video_jnc_myffmpeg_FFmpegUtils.h>
#include <string.h>
#include <time.h>   
#include "libavcodec/avcodec.h"
#include "My_LOG.h"
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
#include <stdio.h>  
#include "libavutil/log.h"  



//Error:(5, 51) module_video_jnc_myffmpeg_FFmpegUtils.h: No such file or directory
JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative
        (JNIEnv *env, jclass clazz){

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}

/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    stringJni
 * Signature: ()Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringJni
        (JNIEnv *env, jclass clazz){
    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}


//Output FFmpeg's av_log()  
void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
}

/*
 * Class:     module_video_jnc_myffmpeg_FFmpegUtils
 * Method:    decode
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_decode
        (JNIEnv *env, jclass clazz, jstring input_jstr, jstring output_jstr) {

    /**
     * AVFormatContext主要存储视音频封装格式中包含的信息，
     * AVInputFormat存储输入视音频封装格式，
     * 每种视音频封装格式都对应一个AVInputFormat结构
     * */
    AVFormatContext *pFormatCtx;


    int i, videoindex;

    /**
     * 每个avStream存储一个视频/音频流的相关数据；每个avstream对应一个AVCodecContext，
     * 存储该视频/音频流使用解码方式相关数据；
     * 每个AVCodecContext中对应一个avcodec，包含该视频/音频对应的解码器。每种解码器都对应一个avcodec结构。
     */
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    //解码后的数据AVFrame
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    //解码前的数据
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    /**
     * 这个格式对应的是图像拉伸，像素格式的转换
     */
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;
    char *input_str = NULL;
    char *output_str = NULL;
    char info[1000] = {0};

    input_str = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    output_str = (*env)->GetStringUTFChars(env, output_jstr, NULL);
    //FFmpeg av_log() callback
    //av_log_set_callback(custom_log);
    LOGE("input %s , output %s ", input_str, output_str);
    /**
     * 初始化，avformat，然后还初始化混合器，分流器。
     */
    av_register_all();
    /**
     * 初始化全局的网络模块，可选的，但是推荐初始化，如果使用了网络协议，必须强制初始化
     */
    avformat_network_init();

    /**
     * AVFormatContext,分配
     */
    pFormatCtx = avformat_alloc_context();

    /**
     * 打开输入流，注意使用avformat_close_input关闭，注意传入的是pFormatCtx的地址（需要用二级指针来保存），
     */
    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    /**
     * 读取文件的信息，第二个参数是AVDictionary（dict.h）,不过推荐使用tree.h中的。因为这个太大效率会低下
     */
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }

    videoindex = -1;

    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        //获取video轨道
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    }


    if (videoindex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;

    /**
     * Find a registered decoder with a matching codec ID.
     * 找到一个解码器，如果没有就返回NULL
     */
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec == NULL) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
   if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
   }

    /**
     * 为解码后存储数据的pframe分配空间
     */
    pFrame = av_frame_alloc();
    /**
     * 将MP4中帧格式转换成yuv，这个pFrameYUV就是用来存储pFrameYUV格式的
     */
    pFrameYUV = av_frame_alloc();

    /**
     * 分配空间
     */
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));

    /**
     * 我也不知道要干嘛，好像是转格式之前设置的一些。
     */
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    /**
     * 为解码前申请空间
     */
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    /**
     * 转格式
     */
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);


    LOGE("[Input     ]%s\n", input_str);
    LOGE("%s[Output    ]%s\n", info, output_str);
    LOGE("%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    LOGE("%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
    LOGE("%s[Resolution]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);

    /**
     * 读写方式打开文件，不是追加
     */
    fp_yuv = fopen(output_str, "wb+");

    if (fp_yuv == NULL) {
        printf("Cannot open output file.\n");
        return -1;
    }

    frame_cnt = 0;
    time_start = clock();

    /**
     * 读取一帧
     */
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoindex) {
            /**
             * 解码
             */
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);

            if (ret < 0) {
                LOGE("Decode Error.\n");
                return -1;
            }
            /**
             * 如果拿到图像帧
             */
            if (got_picture) {
                /**
                 * 格式转换，pFrame-> pFrameYuv
                 */
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                /**
                 * 按yuv420方式写入文件中。
                 */
                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
                //Output info
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        LOGE("decode  %s I" , pictype_str);
                        break;
                    case AV_PICTURE_TYPE_P:
                         LOGE("decode  %s P" , pictype_str);
                        break;
                    case AV_PICTURE_TYPE_B:
                        LOGE("decode  %s B" , pictype_str);
                        break;
                    default:
                        LOGE("decode  %s Other" , pictype_str);
                        break;
                }
                LOGI("Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    /**
     * 清空decoder，如果有数据就写入，没有数据就退出。
     */
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
        {
            LOGE("ret < 0 break ");
            break;
        }

        if (!got_picture){
            LOGE(" got_picture %d break ",got_picture);
            break;
        }


        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                LOGE("I %s" , pictype_str);
                break;
            case AV_PICTURE_TYPE_P:
                LOGE("P %s" , pictype_str);
                break;
            case AV_PICTURE_TYPE_B:
                LOGE("B %s" , pictype_str);
                break;
            default:
                LOGE("Other " );
                break;
        }
        LOGE("Frame Index: %5d. Type :%s ", frame_cnt, pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double) (time_finish - time_start);


    LOGE("%s[Time      ]%fms\n", info, time_duration);
    LOGE("%s[Count     ]%d\n", info, frame_cnt);

    sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);


    (*env)->ReleaseStringUTFChars(env, input_jstr, input_str);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_str);
    return 0;
}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  