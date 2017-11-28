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
#include "myfiler.h"

//https://ffmpeg.org/ffmpeg-filters.html
int main2(const char *inputStr ,const char *outputStr)
{
    int ret;
    AVFrame *frame_in;
    AVFrame *frame_out;
    unsigned char *frame_buffer_in;
    unsigned char *frame_buffer_out;

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    static int video_stream_index = -1;

    //Input YUV
    FILE *fp_in=fopen(inputStr,"rb+");
    if(fp_in==NULL){
        LOGE("Error open input file.\n");
        return -1;
    }
    int in_width=470;
    int in_height=272;

    //Output YUV
    FILE *fp_out=fopen(outputStr,"wb+");
    if(fp_out==NULL){
        LOGE("Error open output file.\n");
        return -1;
    }

    //const char *filter_descr = "lutyuv='u=128:v=128'";
//    const char *filter_descr = "boxblur";
//    const char *filter_descr = "hflip";
    //const char *filter_descr = "hue='h=60:s=-3'";
//    const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
//    const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=white@0.5";
//    const char *filter_descr = "drawtext=fontcolor=green:fontsize=30:text='xhc'";
//    const char *filter_descr = "movie=/storage/emulated/0/FFmpeg/filter.PNG[wm];[in][wm]overlay=5:5[out]";
//    const char *filter_descr = "scale=78:24,transpose=cclock";
    av_log_set_callback(custom_log);
    avfilter_register_all();

    char args[512];
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    /*数据拷贝*/
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in_width,in_height,AV_PIX_FMT_YUV420P,
             1, 25,1,1);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOGE("Cannot create buffer sink\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0){
        LOGE(" avfilter_graph_parse_ptr faildd %d " , ret);
        return ret;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0){
        LOGE(" avfilter_graph_config faildd");
        return ret;
    }

    frame_in=av_frame_alloc();
    frame_buffer_in=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_in->data, frame_in->linesize,frame_buffer_in,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_out=av_frame_alloc();
    frame_buffer_out=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_out->data, frame_out->linesize,frame_buffer_out,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_in->width=in_width;
    frame_in->height=in_height;
    frame_in->format=AV_PIX_FMT_YUV420P;

    while (1) {
        LOGE(" READ frame !");
        if(fread(frame_buffer_in, 1, in_width*in_height*3/2, fp_in)!= in_width*in_height*3/2){
            LOGE(" READ FAILD");
            break;
        }
        //input Y,U,V
        frame_in->data[0]=frame_buffer_in;
        frame_in->data[1]=frame_buffer_in+in_width*in_height;
        frame_in->data[2]=frame_buffer_in+in_width*in_height*5/4;

        if (av_buffersrc_add_frame(buffersrc_ctx, frame_in) < 0) {
            LOGE( "Error while add frame.\n");
            break;
        }

        /* pull filtered pictures from the filtergraph */
        ret = av_buffersink_get_frame(buffersink_ctx, frame_out);
        if (ret < 0){
            LOGE(" av_buffersink_get_frame ret  ");
            break;
        }


        //output Y,U,V
        if(frame_out->format==AV_PIX_FMT_YUV420P){
            for(int i=0;i<frame_out->height;i++){
                fwrite(frame_out->data[0]+frame_out->linesize[0]*i,1,frame_out->width,fp_out);
            }
            for(int i=0;i<frame_out->height/2;i++){
                fwrite(frame_out->data[1]+frame_out->linesize[1]*i,1,frame_out->width/2,fp_out);
            }
            for(int i=0;i<frame_out->height/2;i++){
                fwrite(frame_out->data[2]+frame_out->linesize[2]*i,1,frame_out->width/2,fp_out);
            }
        }
        LOGE("Process 1 frame!\n");
        av_frame_unref(frame_out);
    }
    LOGE(" FILTER END ...");
    fclose(fp_in);
    fclose(fp_out);

    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);

    return 0;
}

int mp4filter(const char *input_str ,const char *output_str){

//    const char *filter_descr = "movie=/storage/emulated/0/FFmpeg/filter.PNG[wm];[in][wm]overlay=5:5[out]";
//    const char *filter_descr = "scale=78:24,transpose=cclock";
//    const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=white@0.5";
    //const char *filter_descr = "lutyuv='u=128:v=128'";
//    const char *filter_descr = "boxblur";
//    const char *filter_descr = "hflip";
    //const char *filter_descr = "hue='h=60:s=-3'";
    const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
//    const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=white@0.5";
//    const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='xhc'";
//    const char *filter_descr = "movie=/storage/emulated/0/FFmpeg/filter.PNG[wm];[in][wm]overlay=5:5[out]";
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
    AVFrame *pFrame ;
    uint8_t *out_buffer;
    //解码前的数据
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;
    char info[1000] = {0};
    //FFmpeg av_log() callback
    av_log_set_callback(custom_log);
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
     * 为解码前申请空间
     */
    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
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

/********************************水印初始化*****************************/

    avfilter_register_all();
    char args[512] ;
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph = avfilter_graph_alloc();
    if(filter_graph == NULL){
        LOGE(" filter_graph  FAILD ");
    }

    AVFilter *buffersrc = avfilter_get_by_name("buffer");
    if(buffersrc == NULL){
        LOGE(" BUFFER SRC FAILD ");
    }
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    if(buffersink == NULL){
        LOGE(" buffersink  FAILD ");
    }
    AVFilterInOut *outputs = avfilter_inout_alloc();
    if(outputs == NULL){
        LOGE(" outputs  FAILD ");
    }

    AVFilterInOut *inputs = avfilter_inout_alloc();
    if(inputs == NULL){
        LOGE(" inputs  FAILD ");
    }
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P , AV_PIX_FMT_NONE};

    AVBufferSinkParams *buffersink_params;

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    /*数据拷贝*/
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             pCodecCtx->width , pCodecCtx->height , AV_PIX_FMT_YUV420P,
             1, 25,1,1);

    ret = avfilter_graph_create_filter(&buffersrc_ctx , buffersrc , "in" , args , NULL , filter_graph);
    if(ret < 0){
        LOGE("create in filter faild");
        return -1 ;
    }

    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx , buffersink , "out",  NULL ,buffersink_params , filter_graph);
    av_free(buffersink_params);
    if(ret < 0){
        LOGE("create out filter faild");
        return -1 ;
    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;


    inputs->name =av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    ret = avfilter_graph_parse_ptr(filter_graph , filter_descr , &inputs , &outputs , NULL);
    if(ret < 0){
        LOGE("avfilter_graph_parse_ptr faild");
        return -1;
    }

    AVFrame *pFrame_out = av_frame_alloc();

    /***************************************水印初始化结束*************************/

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
                pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);
                LOGE(" PTS  %d " , pFrame->pts );

                switch(pFrame->pict_type){
                    case AV_PICTURE_TYPE_I:
                        LOGE(" TYPE I ");
                        break;
                    case AV_PICTURE_TYPE_B:
                        LOGE(" TYPE B ");
                        break;
                    case AV_PICTURE_TYPE_P:
                        LOGE(" TYPE P ");
                        break;
                    default:
                        av_frame_unref(pFrame);
                        continue;
                }


                if(av_buffersrc_add_frame(buffersrc_ctx , pFrame) < 0 ){
                    LOGE("error  while feeding the filtergraph ");
                    break;
                }

//                while(1){
                LOGE(" GET FRAME START ");
                ret = av_buffersink_get_frame(buffersink_ctx ,pFrame_out );
                LOGE("GET FRAME SUCCESS ");
                if(ret < 0){
                    LOGE("av_buffersink_get_frame");
                    break;
                }
                LOGE(" PROCESS 1 FRAME !");
                if(pFrame_out ->format == AV_PIX_FMT_YUV420P){
                    //Y, U, V
                    for(int i=0;i<pFrame_out->height;i++){
                        fwrite(pFrame_out->data[0]+pFrame_out->linesize[0]*i,1,pFrame_out->width,fp_yuv);
                    }
                    for(int i=0;i<pFrame_out->height/2;i++){
                        fwrite(pFrame_out->data[1]+pFrame_out->linesize[1]*i,1,pFrame_out->width/2,fp_yuv);
                    }
                    for(int i=0;i<pFrame_out->height/2;i++){
                        fwrite(pFrame_out->data[2]+pFrame_out->linesize[2]*i,1,pFrame_out->width/2,fp_yuv);
                    }
                }
                av_frame_unref(pFrame_out);
//                }
                frame_cnt++;
            }
            av_frame_unref(pFrame);
        }
        av_free_packet(packet);
    }

    time_finish = clock();
    time_duration = (double) (time_finish - time_start);

    LOGE("%s[Time      ]%fms\n", info, time_duration);
    LOGE("%s[Count     ]%d\n", info, frame_cnt);
    fclose(fp_yuv);
    avfilter_graph_free(&filter_graph);



    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
//    main2(input_str , output_str);
}
