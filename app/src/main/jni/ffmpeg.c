

#include <module_video_jnc_myffmpeg_FFmpegUtils.h>
#include <string.h>
#include <time.h>
#include "libavcodec/avcodec.h"
#include "My_LOG.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <stdio.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include "libavutil/log.h"



//Error:(5, 51) module_video_jnc_myffmpeg_FFmpegUtils.h: No such file or directory
JNIEXPORT jstring JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stringNative
        (JNIEnv *env, jclass clazz) {

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
        (JNIEnv *env, jclass clazz) {
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
 * 解码
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
    //存储编解码器的结构体
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
    av_log_set_callback(custom_log);
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
     * 打开输入流，注意使用avformat_close_input关闭，
     * 注意传入的是pFormatCtx的地址(pFormatCtx本身也是一个指针变量)（需要用二级指针来保存），
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

    for (i = 0; i < pFormatCtx->nb_streams; i++) {
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
                        LOGE("decode  %s I", pictype_str);
                        break;
                    case AV_PICTURE_TYPE_P:
                        LOGE("decode  %s P", pictype_str);
                        break;
                    case AV_PICTURE_TYPE_B:
                        LOGE("decode  %s B", pictype_str);
                        break;
                    default:
                        LOGE("decode  %s Other", pictype_str);
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
        if (ret < 0) {
            LOGE("ret < 0 break ");
            break;
        }

        if (!got_picture) {
            LOGE(" got_picture %d break ", got_picture);
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
                LOGE("I %s", pictype_str);
                break;
            case AV_PICTURE_TYPE_P:
                LOGE("P %s", pictype_str);
                break;
            case AV_PICTURE_TYPE_B:
                LOGE("B %s", pictype_str);
                break;
            default:
                LOGE("Other ");
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

/*
* Class:     module_video_jnc_myffmpeg_FFmpegUtils
* Method:    stream
* Signature: (Ljava/lang/String;Ljava/lang/String;)I
* 推流
*/
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_stream
        (JNIEnv *env, jclass clazz, jstring input_jstr, jstring output_jstr) {

    AVOutputFormat *ofmt = NULL;
    /**
       * AVFormatContext主要存储视音频封装格式中包含的信息，
       * AVInputFormat存储输入视音频封装格式，
       * 每种视音频封装格式都对应一个AVInputFormat结构
       * */
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    //解码前用来保存数据的
    AVPacket pkt;

    int ret, i;
    const char *input_str = NULL;
    const char *output_str = NULL;
    char info[1000] = {0};

    input_str = (*env)->GetStringUTFChars(env, input_jstr, NULL);
    output_str = (*env)->GetStringUTFChars(env, output_jstr, NULL);

    LOGE(" input str %s , output str %s ", input_str, output_str);

    //设置日志
    av_log_set_callback(custom_log);
    //初始化全部
    av_register_all();
    //网络初始化，要推流必须初始化
    avformat_network_init();
    //打开输入文件，从sdcard上读取视频
    if ((ret = avformat_open_input(&ifmt_ctx, input_str, 0, 0)) < 0) {
        LOGE("Could not open input file.");
        goto end;
    }
    //获取输入流
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        LOGE("Failed to retrieve input stream information");
        goto end;
    }

    int videoindex = -1;
    /**
     * ifmt_ctx->nb_streams
     * 一个AVFormatContext可能有很多个stream，（视频，音频，字幕等。）
     */
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    /**
     * 为ofmt_ctx分配空间
     */
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", output_str); //RTMP
    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", output_str);//UDP

    if (!ofmt_ctx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            LOGE("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //Copy the settings of AVCodecContext
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            LOGE("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    //Open output URL
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, output_str, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("Could not open output URL '%s'", output_str);
            goto end;
        }
    }
    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        LOGE("Error occurred when opening output URL\n");
        goto end;
    }

    int frame_index = 0;

    int64_t start_time = av_gettime();
    while (1) {
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (pkt.pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
            //Parameters
            pkt.pts = (double) (frame_index * calc_duration) /
                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
            pkt.dts = pkt.pts;
            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
        }
        //Important:Delay
        if (pkt.stream_index == videoindex) {
            AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);
        }
        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Print to Screen
        if (pkt.stream_index == videoindex) {
            LOGE("Send %8d video frames to output URL\n", frame_index);
            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

        if (ret < 0) {
            LOGE("Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }

    //Write file trailer
    av_write_trailer(ofmt_ctx);
    end:
    avformat_close_input(&ifmt_ctx);
    (*env)->ReleaseStringUTFChars(env, input_jstr, input_str);
    (*env)->ReleaseStringUTFChars(env, output_jstr, output_str);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOGE("Error occurred.\n");
        return -1;
    }
    return 0;
}

/*
* Class:     module_video_jnc_myffmpeg_FFmpegUtils
* Method:    encode
* Signature: (Ljava/lang/String;)I
* 将MP4格式的数据转码成flv（编码格式也改变下h264，改变成随便一个格式。）
 * 转码是先h264 -》 yuv -》 其他格式
*/
JNIEXPORT jint JNICALL Java_module_video_jnc_myffmpeg_FFmpegUtils_encode
        (JNIEnv *env, jclass clazz, jstring jstr_inputPath, jstring jstr_outPath) {

    const char *input_str = (*env)->GetStringUTFChars(env, jstr_inputPath, NULL);
    const char *output_str = (*env)->GetStringUTFChars(env, jstr_outPath, NULL);
    char real_output[100] = {0};
    strcat(real_output, output_str);
    strcat(real_output, "/deocede_yuv.yuv");

    LOGE(" input_str %s ,output str %s ", input_str, real_output);

    AVFormatContext *pFormatCtx;
    int i, videoIndex = -1;
    AVCodecContext *pCodeCtx;
    AVCodec *pCodec;
    AVCodec *pEncode;
    AVFrame *pFrameMP4, *pFrameYUV;
    uint8_t *out_buffer;
    uint8_t *flv_output_buffer;
    AVPacket *packet;
    int ret, got_pic;
    /**
     * 后面转换可能会用到
     */
    struct SwsContext *img_convert_ctx;

    clock_t time_start, time_finish;
    double time_duration = 0.0;
    char info[1000] = {0};

    av_log_set_callback(custom_log);
    av_register_all();
    avformat_network_init();
    //先分配pFormatCtx
    pFormatCtx = avformat_alloc_context();

    //先为input 初始化pFormatCtx
    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE(" can't open input stream ");
        return -1;
    }

    //获取流信息，对于一些没有头文件的也有用，文件不会被这个函数改动
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE(" couldn't find stream information . \n");
        return -1;
    }

    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGE(" find video index success");
            videoIndex = i;
            break;
        }
    }

    if (videoIndex == -1) {
        LOGE(" find video index faild");
        return -1;
    }

    //这个接口好像已经des。。。后面再来处理warning
    pCodeCtx = pFormatCtx->streams[videoIndex]->codec;

    LOGE(" PCODE CODE ID %d ", pCodeCtx->codec_id);

    pCodec = avcodec_find_decoder(pCodeCtx->codec_id);

    if (pCodec == NULL) {
        LOGE(" couldn't decoder find codec ");
        return -1;
    }

    LOGE("  find decoder success ");

    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        LOGE(" open decoder faild ");
        return -1;
    }
    LOGE(" open decoder success ");

    //保存帧分配空间
    pFrameMP4 = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    LOGE(" pCodeCtx->width %d , pCodeCtx->height %d ", pCodeCtx->width, pCodeCtx->height);
    out_buffer = (unsigned char *) av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodeCtx->width, pCodeCtx->height, 1));


    if (out_buffer == NULL) {
        LOGE(" open out_buffer faild ");
        return -1;
    }
    LOGE(" open out_buffer success ");



    /**
     * 对 pFrameYUV 的初始化
     */
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodeCtx->width, pCodeCtx->height, 1);

    LOGE(" pFrameYUV->linesize = %d , w * h = %d", pFrameYUV->linesize,
         (pCodeCtx->width * pCodeCtx->height));

    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    /**
     * 分配空间
     */
    img_convert_ctx = sws_getContext(pCodeCtx->width, pCodeCtx->height, pCodeCtx->pix_fmt,
                                     pCodeCtx->width, pCodeCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    LOGE(" pformatctx %s ", pFormatCtx->iformat->name);
    LOGE(" codec %s ", pCodeCtx->codec->name);
    //追加字符，保证第一个参数空间够大。
    FILE *out_yuv = fopen(real_output, "wb+");

    if (out_yuv == NULL) {
        LOGE(" OPEN OUTPUT FILE FAILD ");
        return -1;
    }

//    char flv_output[100] = {0};
//
//    strcat(flv_output , output_str);
//    strcat(flv_output , "/mp4convert.flv");
//
//    ret = avformat_alloc_output_context2(&pFormatCtx, NULL, "flv", flv_output);
//
//    if (ret < 0) {
//        LOGE(" avformat_alloc_output_context2 faild ");
//        return -1;
//    }
//
//    ret = avio_open(&pFormatCtx->pb, flv_output, AVIO_FLAG_WRITE);
//
//
//    if (ret < 0) {
//        LOGE("Could not open output URL '%s'", output_str);
//        return -1;
//    }
//
//    AVStream* video_st;


//    video_st = avformat_new_stream(pFormatCtx , 0);
//
//    if(video_st == NULL){
//        LOGE(" open video st faild");
//        return -1;
//    }
//    LOGE(" PENCODE ID %d " , pFormatCtx->video_codec->id);
//    pEncode = avcodec_find_encoder(pFormatCtx->video_codec->id);
//
//    if(pEncode == NULL){
//        LOGE(" find encode faild");
//        return -1;
//    }

//    ret = avcodec_open2(pCodeCtx , pEncode , NULL);

//    if(ret < 0){
//        LOGE(" open encode faild ");
//        return -1;
//    }

//    ret = avformat_write_header(pFormatCtx , NULL);

//    LOGE(" avformat_write_header return %d  " , ret);

//    if(ret != 0 && ret != 1){
//        LOGE(" avformat_write_header falid " , ret);
//        return -1;
//    }

//    LOGE(" avformat_write_header success " , ret);

    int frame_cnt = 0;
    time_start = clock();
    int y_size = 0;

    while (av_read_frame(pFormatCtx, packet) >= 0) {

        if (packet->stream_index == videoIndex) {
            ret = avcodec_decode_video2(pCodeCtx, pFrameMP4, &got_pic, packet);
            if (ret < 0) {
                LOGE("DECODE ERROR ");
                return -1;
            }

            if (got_pic) {
                LOGE("get pic %d ", frame_cnt);
                /**
                 * 格式转换
                 */
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrameMP4->data,
                          pFrameMP4->linesize,
                          0, pCodeCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                y_size = pCodeCtx->width * pCodeCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, out_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, out_yuv);
                fwrite(pFrameYUV->data[2], 1, y_size / 4, out_yuv);
                frame_cnt++;
            }
        }

        //记得释放，然后重新装载
        av_free_packet(packet);
    }

    (*env)->ReleaseStringUTFChars(env, jstr_inputPath, input_str);
    (*env)->ReleaseStringUTFChars(env, jstr_outPath, output_str);
    return 0;
}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  