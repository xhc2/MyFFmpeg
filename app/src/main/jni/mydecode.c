//
// Created by dugang on 2017/11/27.
//
#include <string.h>
#include "My_LOG.h"
#include <time.h>
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include "libavutil/log.h"
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include "mydecode.h"


int encode_h264(char *input_str , char *output_str){
    char real_output[100] = {0};
    strcat(real_output, output_str);
    strcat(real_output, "/deocede_yuv.yuv");

    LOGE(" input_str %s ,output str %s ", input_str, real_output);

    AVFormatContext *pFormatCtx;
    int i, videoIndex = -1;
    AVCodecContext *pCodeCtx;
    AVCodec *pCodec;
    AVFrame *pFrameMP4/*, *pFrameYUV*/;
    uint8_t *out_buffer;
    AVPacket *packet;
    int ret, got_pic;
    /**
     * 后面转换可能会用到
     */
    struct SwsContext *img_convert_ctx;

    clock_t time_start, time_finish;
    double time_duration = 0.0;

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

    if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
        LOGE(" open decoder faild ");
        return -1;
    }

    //保存帧分配空间
    pFrameMP4 = av_frame_alloc();
    LOGE(" pCodeCtx->width %d , pCodeCtx->height %d ", pCodeCtx->width, pCodeCtx->height);
    out_buffer = (unsigned char *) av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodeCtx->width, pCodeCtx->height, 1));


    if (out_buffer == NULL) {
        LOGE(" open out_buffer faild ");
        return -1;
    }
    LOGE(" open out_buffer success ");




    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    /**
     * 分配空间
     */
    img_convert_ctx = sws_getContext(pCodeCtx->width, pCodeCtx->height, pCodeCtx->pix_fmt,
                                     pCodeCtx->width, pCodeCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);

    LOGE(" pformatctx %s ", pFormatCtx->iformat->name);
    LOGE(" codec %s ", pCodeCtx->codec->name);
    FILE *out_yuv = fopen(real_output, "wb+");

    if (out_yuv == NULL) {
        LOGE(" OPEN OUTPUT FILE FAILD ");
        return -1;
    }

    char flv_output[100] = {0};
    strcat(flv_output , output_str);
    strcat(flv_output , "/mp4convert.flv");
    AVFormatContext *pOFC ;
    AVOutputFormat *oft ;
    oft = av_guess_format(NULL , flv_output , NULL);
    pOFC = avformat_alloc_context();

    if(pOFC == NULL){
        LOGE(" POFG FAILD ");
        return -1;
    }

    if(oft == NULL){
        LOGE(" guess fmt faild ");
        return -1;
    }
    LOGE(" FORMAT NAME %s ",oft->name);
    pOFC->oformat = oft;

    ret = avio_open(&pOFC->pb , flv_output ,AVIO_FLAG_READ_WRITE );

    if(ret < 0){
        LOGE("AVIO OPEN FAILD ");
        return -1;
    }

    AVStream* video_st;

    video_st = avformat_new_stream(pOFC , 0);

    if(video_st == NULL){
        LOGE(" avformat_new_stream NULL ");
        return -1;
    }
    if(video_st->codec == NULL){
        LOGE(" video_st->codec NULL ");
        return -1;
    }

    video_st->codec->codec_id = pOFC->oformat->video_codec;//oft->video_codec;
    video_st->codec->codec_type=AVMEDIA_TYPE_VIDEO;
    video_st->codec->pix_fmt=AV_PIX_FMT_YUV420P;
    video_st->codec->width = pCodeCtx->width;
    video_st->codec->height = pCodeCtx->height;
    video_st->codec->bit_rate = 400000;
    video_st->codec->gop_size = 250;
    video_st->codec->time_base.num = 1;
    video_st->codec->time_base.den = 25;
    video_st->codec->qmin = 10;
    video_st->codec->qmax = 51;
//    video_st->codec->max_b_frames = 3;
    LOGE(" PCODEC ID %d ", video_st->codec->codec_id);

    if(video_st->codec->codec_id == AV_CODEC_ID_NONE){
        LOGE(" AV_CODEC_ID_NONE ");
        return -1;
    }

    AVCodec *pCEncode = avcodec_find_encoder(video_st->codec->codec_id);

    if(pCEncode == NULL){
        LOGE(" find  Encode faild ");
        return -1 ;
    }
    LOGE(" ENCODE NAME %s ",pCEncode->name);

    ret = avcodec_open2( video_st->codec,  pCEncode , NULL);

    if(ret < 0){
        LOGE(" open encode faild ");
        return -1;
    }

    AVFrame *pFrameFLV ;

    pFrameFLV = av_frame_alloc();

    if(pFrameFLV ==  NULL){
        LOGE(" pFrameFLV ALLOC FAILD ");
        return -1;
    }

    int picture_size = avpicture_get_size(pCodeCtx->pix_fmt , pCodeCtx->width , pCodeCtx->height);

    LOGE(" picture size %d " , picture_size);

    uint8_t *picture_buff = (uint8_t *)av_malloc(picture_size);

    if(picture_buff == NULL){
        LOGE(" picture_buff MALLOC FAILD  ");
        return -1;
    }

    LOGE(" PIX_FMT %d , width %d , height %d" , pCodeCtx->pix_fmt, pCodeCtx->width , pCodeCtx->height);

    /**
     * 对 pFrameFLV 的初始化
     */

//    av_image_fill_arrays(pFrameFLV->data, pFrameFLV->linesize, picture_buff, AV_PIX_FMT_YUV420P,
//                         pCodeCtx->width, pCodeCtx->height, 1);

    avpicture_fill((AVPicture *)pFrameFLV, picture_buff, video_st->codec->pix_fmt, video_st->codec->width, video_st->codec->height);
    avformat_write_header(pOFC , NULL);

    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket) );

    ret = av_new_packet(avPacket , 500);

//    av_init_packet(avPacket);

    if(ret < 0){
        LOGE(" avPacket ALLOC FAILD  ");
        return -1 ;
    }

    int y_size = pCodeCtx->width * pCodeCtx->height;

    int frame_cnt = 0;
    time_start = clock();
    pFrameFLV->format = pCodeCtx->pix_fmt;
    pFrameFLV->width  = pCodeCtx->width;
    pFrameFLV->height = pCodeCtx->height;

    //从MP4文件中读取一帧,保存在packet中
    while (av_read_frame(pFormatCtx, packet) >= 0) {

        LOGE(" read a frame !");
        if (packet->stream_index == videoIndex) {
            //将h264解码成yuv，pFrameMP4已经是yuv数据了。后面再来改变量名。
            ret = avcodec_decode_video2(pCodeCtx, pFrameMP4, &got_pic, packet);
            LOGE(" DEOCDE ret %d , gotpic %d " , ret , got_pic);
            if (ret < 0) {
                LOGE("DECODE ERROR ");
                return -1;
            }
            if(got_pic < 0){
                LOGE(" got pic faild ");
                return -1;
            }
            pFrameFLV->data[0] = pFrameMP4->data[0];
            pFrameFLV->data[1] = pFrameMP4->data[1];
            pFrameFLV->data[2] = pFrameMP4->data[2];
            pFrameFLV->pts = pFrameMP4->pts;

//            fwrite(pFrameFLV->data[0] ,1 ,y_size,  ftest);
//            fwrite(pFrameFLV->data[1], 1, y_size / 4, ftest);  //U
//            fwrite(pFrameFLV->data[2], 1, y_size / 4, ftest);  //V

            LOGE(" CODE W %d , H %d , Frame w %d ,h %d " ,video_st->codec->width,video_st->codec->height ,
                 pFrameFLV->width , pFrameFLV->height);

            //然后将yuv编码成flv,
            ret = avcodec_encode_video2(video_st->codec , avPacket , pFrameFLV , &got_pic);

            LOGE(" encode ret %d , gotpic %d " , ret , got_pic);

            if(ret < 0){
                LOGE(" avcodec_encode_video2 faild ");
                return -1;
            }

            if(got_pic == 1){
                LOGE(" SUCCESS TO ENCODE FRAME ");
                ret = av_write_frame(pOFC , avPacket);
                if(ret < 0){
                    LOGE(" WRITE FRAME FAILD ");
                    return -1;
                }
                av_free_packet(avPacket);
            }
        }

        //记得释放，然后重新装载
        av_free_packet(packet);
    }
//    fclose(ftest);
    //Write file trailer
    av_write_trailer(pOFC);

    LOGE(" END .....");
}
int encodeYuv(char *input_str, char *output_str) {
    int width = 480, height = 272;
    int ret = 0;
    LOGE(" input path %s , output path %s ", input_str, output_str);
    FILE *iFile = fopen(input_str, "r");
    if (iFile == NULL) {
        LOGE(" OPEN INPUT FILE FAILD! ");
        return -1;
    }


    AVFormatContext *pOFC;
    AVOutputFormat *oft;
    oft = av_guess_format(NULL, output_str, NULL);
    pOFC = avformat_alloc_context();

    if (pOFC == NULL) {
        LOGE(" POFG FAILD ");
        return -1;
    }
    if (oft == NULL) {
        LOGE(" guess fmt faild ");
        return -1;
    }
    LOGE(" FORMAT NAME %s ", oft->name);
    pOFC->oformat = oft;
    ret = avio_open(&pOFC->pb, output_str, AVIO_FLAG_READ_WRITE);
    if (ret < 0) {
        LOGE(" avio_open faild! ");
        return -1;
    }
    AVStream *video_st;

    video_st = avformat_new_stream(pOFC, 0);

    if (video_st == NULL) {
        LOGE(" video_st FAILD !");
        return -1;
    }
    int framecnt = 0;
    video_st->codec->codec_id = pOFC->oformat->video_codec;
    video_st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    video_st->codec->pix_fmt = AV_PIX_FMT_YUV420P;
    video_st->codec->width = width;
    video_st->codec->height = height;
    video_st->codec->bit_rate = 400000;
    video_st->codec->gop_size = 250;
    video_st->codec->time_base.num = 1;
    video_st->codec->time_base.den = 25;
    video_st->codec->qmin = 10;
    video_st->codec->qmax = 51;

    AVCodec *pCodec = avcodec_find_encoder(video_st->codec->codec_id);
    if (pCodec == NULL) {
        LOGE("  pCodec null ");
        return -1;
    }

    if (avcodec_open2(video_st->codec, pCodec, NULL) < 0) {
        printf("Failed to open encoder! \n");
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    int pic_size = avpicture_get_size(video_st->codec->pix_fmt, video_st->codec->width,
                                      video_st->codec->height);
    LOGE(" pic_size %d ", pic_size);
    uint8_t *picture_buf = (uint8_t *) av_malloc(pic_size);
    avpicture_fill((AVPicture *) pFrame, picture_buf, video_st->codec->pix_fmt,
                   video_st->codec->width, video_st->codec->height);

    //Write File Header
    avformat_write_header(pOFC, NULL);
    AVPacket *pkt = (AVPacket *) av_malloc(sizeof(AVPacket));

    av_new_packet(pkt, pic_size);

    int y_size = video_st->codec->width * video_st->codec->height;
    int i = 0;
    while (fread(picture_buf, 1, y_size * 3 / 2, iFile)) {
        pFrame->data[0] = picture_buf;              // Y
        pFrame->data[1] = picture_buf + y_size;      // U
        pFrame->data[2] = picture_buf + y_size * 5 / 4;  // V
        //PTS
        //pFrame->pts=i;
        pFrame->pts = i * (video_st->time_base.den) / ((video_st->time_base.num) * 25);

        int got_picture = 0;
        //Encode
        int ret = avcodec_encode_video2(video_st->codec, pkt, pFrame, &got_picture);
        if (ret < 0) {
            LOGE(" FAILD ENCODE ");
            return -1;
        }

        if (got_picture == 1) {
            LOGE(" ENCODE success %d", framecnt);
            framecnt++;
            pkt->stream_index = video_st->index;
            ret = av_write_frame(pOFC, pkt);
            av_free_packet(pkt);

        }
        ++i;
    }
    av_write_trailer(pOFC);
    LOGE(" end... ");

    //Clean
    if (video_st) {
        avcodec_close(video_st->codec);
        av_free(pFrame);
        av_free(picture_buf);
    }
    avio_close(pOFC->pb);
    avformat_free_context(pOFC);
    fclose(iFile);
}

int decode(const char *input_str,const char *output_str) {

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
    AVFrame *pFrame/*, *pFrameYUV*/;

    uint8_t *out_buffer;
    //解码前的数据
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    /**
     * 这个格式对应的是图像拉伸，像素格式的转换
     */
//    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;

    char info[1000] = {0};

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
//    pFrameYUV = av_frame_alloc();

    /**
     * 分配空间
     */
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    /**
     * 我也不知道要干嘛，好像是转格式之前设置的一些。
     */
//    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
//                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
    /**
     * 为解码前申请空间
     */
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    /**
     * 转格式
     */
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
//                                     SWS_BICUBIC, NULL, NULL, NULL);


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
                int lenY = malloc_usable_size(pFrame->data[0]);
                int lenU = malloc_usable_size(pFrame->data[1]);
                int lenV = malloc_usable_size(pFrame->data[2]);
                LOGE("解码 %d , %d , %d y_size=%d ", lenY, lenU, lenV, y_size);
                /**
                 * 按yuv420方式写入文件中。
                 */
                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrame->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrame->data[1], 1, y_size / 4, fp_yuv);  //U
                fwrite(pFrame->data[2], 1, y_size / 4, fp_yuv);  //V
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

//        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
//                  pCodecCtx->height,
//                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrame->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrame->data[1], 1, y_size / 4, fp_yuv);  //U
        fwrite(pFrame->data[2], 1, y_size / 4, fp_yuv);  //V
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
//    sws_freeContext(img_convert_ctx);
    fclose(fp_yuv);

    av_frame_free(&pFrame);
//    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
