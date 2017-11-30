
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <My_LOG.h>
#include <unistd.h>

/*
 * 将MP4解码成yuv然后打上水印，然后再编码成h264的mp4格式。
 */
const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=white@0.5";

AVFormatContext *fmt_ctx;
AVCodecContext *dec_ctx;
AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
int video_stream_index = -1;
int64_t last_pts = AV_NOPTS_VALUE;

//将yuv打包成mp4格式输出到sdcard上
AVPacket *pkt;
//AVFrame *pFrame;
AVFormatContext *pOFC;
AVOutputFormat *oft;
AVStream *video_st;
int open_input_file(const char *filename)
{
    int ret;
    AVCodec *dec;
    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        LOGE( "Cannot open input file\n");
        return ret;
    }
    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        LOGE(  "Cannot find stream information\n");
        return ret;
    }
    /* select the video stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGE( "Cannot find a video stream in the input file\n");
        return ret;
    }
    video_stream_index = ret;
    dec_ctx = fmt_ctx->streams[video_stream_index]->codec;

    LOGE(" CODE NAME  %s , PIX %d " ,dec->name , dec_ctx->pix_fmt);
    //解码
    av_opt_set_int(dec_ctx, "refcounted_frames", 1, 0);
    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        LOGE(  "Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

int init_filters(const char *filters_descr)
{
    char args[512];
    int ret = 0;
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVRational time_base = fmt_ctx->streams[video_stream_index]->time_base;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
             time_base.num, time_base.den,
             dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create buffer source\n");
        goto end;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        LOGE( "Cannot create buffer sink\n");
        goto end;
    }
    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE( "Cannot set output pixel format\n");
        goto end;
    }
    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */
    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;
    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
}

void display_frame(const AVFrame *frame, AVRational time_base  ,FILE *output)
{
    int x, y;
    uint8_t *p0, *p;
    int64_t delay;

    if (frame->pts != AV_NOPTS_VALUE) {
        if (last_pts != AV_NOPTS_VALUE) {
            /* sleep roughly the right amount of time;
             * usleep is in microseconds, just like AV_TIME_BASE. */
            delay = av_rescale_q(frame->pts - last_pts,
                                 time_base, AV_TIME_BASE_Q);
            if (delay > 0 && delay < 1000000)
                usleep(delay);
        }
        last_pts = frame->pts;
    }
    int y_size = frame->width * frame->height;
    encode_frame(frame , pkt , video_st);



//    fwrite(frame->data[0], 1, y_size, output);
//    fwrite(frame->data[1], 1, y_size / 4, output);
//    fwrite(frame->data[2], 1, y_size / 4, output);
}



int init_output(const char* output_path , int width , int height){
    int ret = 0;

    oft = av_guess_format(NULL, output_path, NULL);
    pOFC = avformat_alloc_context();

    if (pOFC == NULL) {
        LOGE(" POFG FAILD ");
        return -1;
    }
    if (oft == NULL) {
        LOGE(" guess fmt faild ");
        return -1;
    }
    pOFC->oformat = oft;

    ret = avio_open(&pOFC->pb, output_path, AVIO_FLAG_READ_WRITE);



    video_st = avformat_new_stream(pOFC, 0);

    if (video_st == NULL) {
        LOGE(" video_st FAILD !");
        return -1;
    }

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

//    pFrame = av_frame_alloc();

    int pic_size = avpicture_get_size(video_st->codec->pix_fmt, video_st->codec->width,
                                      video_st->codec->height);
    LOGE(" pic_size %d ", pic_size);
//    uint8_t *picture_buf = (uint8_t *) av_malloc(pic_size);
//    avpicture_fill((AVPicture *) pFrame, picture_buf, video_st->codec->pix_fmt,
//                   video_st->codec->width, video_st->codec->height);

    //Write File Header
    avformat_write_header(pOFC, NULL);
    pkt = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt, pic_size);


    return ret;
}

int encode_frame(AVFrame *frame , AVPacket *pkt ,  AVStream *stream){
    int got_picture = 0 ;
    int ret = avcodec_encode_video2(stream->codec, pkt, frame, &got_picture);
    if (ret < 0) {
        LOGE(" FAILD ENCODE ");
        return -1;
    }
    if (got_picture == 1) {
        LOGE(" ENCODE success %d");
        pkt->stream_index = stream->index;
        ret = av_write_frame(pOFC, pkt);
        av_free_packet(pkt);
    }
}

//给video打水印
int filter_video(const char* input_path , const char* output_path){

    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();
    FILE *oFile = fopen(output_path , "wb+");
    int ret  = 0;
    av_register_all();
    avfilter_register_all();

    if(oFile == NULL){
        LOGE("cant open output file !");
        return -1;
    }

    int got_frame;
    if (!frame || !filt_frame) {
        LOGE("Could not allocate frame");
        exit(1);
    }

    if((ret = open_input_file(input_path)) < 0){
        LOGE("open input file faild ");
        return -1;
    }

    if((ret = init_filters(filter_descr)) < 0){
        LOGE("init_filters faild ");
        return -1;
    }
    if((ret = init_output(output_path ,dec_ctx->width , dec_ctx->height))){
        LOGE("init_output faild ");
        return -1;
    }

    int framecnt = 0;
    /* read all packets */
    while (1) {
        if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
            break;

        if (packet.stream_index == video_stream_index) {
            LOGE("read a frame !");
            got_frame = 0;
            ret = avcodec_decode_video2(dec_ctx, frame, &got_frame, &packet);
            if (ret < 0) {
                LOGE("Error decoding video\n");
                break;
            }
            if (got_frame) {
                framecnt ++;
                frame->pts = av_frame_get_best_effort_timestamp(frame);
                /* push the decoded frame into the filtergraph */
                if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                    LOGE("Error while feeding the filtergraph\n");
                    break;
                }
                /* pull filtered frames from the filtergraph */
                while (1) {
                    ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    if (ret < 0)
                        goto end;
                    display_frame(filt_frame, buffersink_ctx->inputs[0]->time_base , oFile);
                    av_frame_unref(filt_frame);
                }
                av_frame_unref(frame);
            }
        }
        av_packet_unref(&packet);
    }

    av_write_trailer(pOFC);

    end:
    avfilter_graph_free(&filter_graph);
    avcodec_close(dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    fclose(oFile);

    //Clean
    if (video_st) {
        avcodec_close(video_st->codec);
    }
    avio_close(pOFC->pb);
    avformat_free_context(pOFC);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOGE("Error occurred: %s\n", av_err2str(ret));

    }
    LOGE(" end ...%d " , framecnt);
    return ret;
}