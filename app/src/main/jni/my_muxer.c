//
// Created by dugang on 2017/12/4.
//
#include <libavformat/avformat.h>
#include <My_LOG.h>
#include "my_muxer.h"
#include "libavutil/mathematics.h"
/**
 * 分离器
 * @param input_path ，源文件路径
 * @param out_v_path ，输出的视频路劲
 * @param out_a_path ，输出的音频路劲
 * @return
 * http://blog.csdn.net/leixiaohua1020/article/details/39802819
 */
int demuxer(const char* input_path , const char *out_v_path , const char *out_a_path){

    AVOutputFormat *ofmt_a = NULL , *ofmt_v = NULL;

    AVFormatContext *ifmt_ctx = NULL , *ofmt_ctx_a = NULL , *ofmt_ctx_v = NULL;

    AVPacket pkt;
    int ret = 0;
    int videoindex = -1, audioindex = -1;
    int frame_index = 0 ;
    av_register_all();
    av_log_set_callback(custom_log);

    //打开输入的文件
    ret = avformat_open_input(&ifmt_ctx  ,input_path , 0 , 0);
    if(ret < 0){
        LOGE("CANT OPEN INPUT !");
        return -1;
    }
    //找到输入流
    ret = avformat_find_stream_info(ifmt_ctx , 0);
    if(ret < 0){
        LOGE("avformat_find_stream_info faild !");
        return -1;
    }
    //打开输出文件 用来保存视频裸流（e.g h264）
    avformat_alloc_output_context2(&ofmt_ctx_v , NULL , NULL , out_v_path);
    if(ofmt_ctx_v == NULL){
        LOGE("avformat_alloc_output_context2 V faild !");
        return -1;
    }
    ofmt_v =ofmt_ctx_v->oformat;
    //打开输出文件，用来保存音频裸流（e.g aac）
    avformat_alloc_output_context2(&ofmt_ctx_a , NULL , NULL , out_a_path);

    if(ofmt_ctx_a == NULL){
        LOGE("avformat_alloc_output_context2 a faild !");
        return -1;
    }

    ofmt_a = ofmt_ctx_a->oformat;

    for(int i = 0 ; i < ifmt_ctx->nb_streams ; ++ i){
        AVStream *outStream;
        AVFormatContext *ofmt_ctx;
        if(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            LOGE("VIDEO TYPE ");
            videoindex = i;
            //增加一个视屏流到ofmt_ctx_v ， 并且添加一个编解码器
            outStream = avformat_new_stream(ofmt_ctx_v ,ifmt_ctx->streams[i]->codec->codec);
            ofmt_ctx = ofmt_ctx_v;
        }
        else if(ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            LOGE("AUDIO TYPE ");
            audioindex = i;
            outStream = avformat_new_stream(ofmt_ctx_a ,ifmt_ctx->streams[i]->codec->codec);
            ofmt_ctx = ofmt_ctx_a;
        }
        else{
            LOGE(" BREAK ");
            break;
        }

        if(!outStream){
            LOGE("outStream null");
            return -1;
        }
        /**
         * 复制avcodec的setting
         */
        if((ret = avcodec_copy_context(outStream->codec , ifmt_ctx->streams[i]->codec)) < 0){
            LOGE(" AVCODEC COPY CONTXT FAILD !");
            return -1;
        }
        outStream->codec->codec_tag = 0;

        /**
         * AVFMT_GLOBALHEADER   Format wants global header.这种格式需要总体header
         * 对应的flag，每一位应该有对应的意思，如果&上不为0说明需要这个global header
         */
        if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
            //CODEC_FLAG_GLOBAL_HEADER-> Place global headers in extradata instead of every keyframe.
            outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }

//    av_dump_format(ifmt_ctx , 0 , input_path , 0);
//    av_dump_format(ofmt_ctx_v , 0 , out_v_path , 1);
//    av_dump_format(ofmt_ctx_a , 0 , out_a_path , 1);

    /**
     * AVFMT_NOFILE
     *  Demuxer will use avio_open, no opened file should be provided by the caller.
     */
    if(!(ofmt_v->flags & AVFMT_NOFILE)){
        if(avio_open(&ofmt_ctx_v->pb , out_v_path , AVIO_FLAG_WRITE) < 0){
            LOGE(" AVIO OPEN VIDEO FAILD ");
            return -1;
        }
    }

    if(!(ofmt_a->flags & AVFMT_NOFILE)){
        if(avio_open(&ofmt_ctx_a->pb , out_a_path , AVIO_FLAG_WRITE) < 0){
            LOGE(" AVIO OPEN audio FAILD ");
            return -1;
        }
    }

    if(avformat_write_header(ofmt_ctx_v ,  NULL ) < 0){
        LOGE(" avformat_write_header V FAIld ");
        return -1;
    }

    if(avformat_write_header(ofmt_ctx_a ,  NULL ) < 0){
        LOGE(" avformat_write_header a faild ");
        return -1;
    }

    /**
     * mp4，flv,mkv中的h264 ， 需要用到h264_mp4toannexb的bitstreamfilter
     */
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");

    enum AVRounding AVROUNDING = AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX ;

    while(1){

        AVOutputFormat *ofmt_ctx;
        AVStream *inStream , *outStream;
        AVFormatContext *afc ;
        //从文件中读取一帧
        if(av_read_frame(ifmt_ctx , &pkt) < 0){
            LOGE(" READ FRAME < 0 ");
            break;
        }
        inStream = ifmt_ctx->streams[pkt.stream_index];
        //是视频帧
        if(pkt.stream_index == videoindex){
            outStream = ofmt_ctx_v->streams[0];
            ofmt_ctx = ofmt_ctx_v->oformat;
            afc = ofmt_ctx_v;
            av_bitstream_filter_filter(h264bsfc, inStream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
            LOGE(" video ...");
        }
        //是音频帧
        else if(pkt.stream_index == audioindex){
            outStream = ofmt_ctx_a->streams[0];
            ofmt_ctx = ofmt_ctx_a->oformat;
            afc = ofmt_ctx_a;
            LOGE(" audio ...");
        }
        else{
            LOGE(" CONTINUE ...");
            continue;
        }

        //Convert PTS/DTS
        //pts 显示时间戳，dts，解码时间戳
        //http://blog.csdn.net/fssssssss/article/details/44624847
        //outStream->time_base 时间基，一个时间单位
        // av_rescale_q_rnd = `a * bq / cq`.
        pkt.pts = av_rescale_q_rnd(pkt.pts, inStream->time_base, outStream->time_base, AVROUNDING);
        pkt.dts = av_rescale_q_rnd(pkt.dts, inStream->time_base, outStream->time_base, AVROUNDING);
        pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
        pkt.pos = -1;
        pkt.stream_index=0;

        /**
         * 交叉的写入媒体文件中
         */
        if(av_interleaved_write_frame(afc, &pkt) < 0){
            LOGE(" WRATE FAILD ");
            break;
        }
        av_free_packet(&pkt);
        frame_index++;
        LOGE("frame_index %d " , frame_index);
    }

    av_bitstream_filter_close(h264bsfc);
    av_write_trailer(ofmt_ctx_a);
    av_write_trailer(ofmt_ctx_v);
    avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx_a && !(ofmt_a->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx_a->pb);

    if (ofmt_ctx_v && !(ofmt_v->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx_v->pb);

    avformat_free_context(ofmt_ctx_a);
    avformat_free_context(ofmt_ctx_v);

    LOGE("------END-----");
    return  ret;

}


/**
 * 混合器
 * @param output_path 输出的mp4路劲
 * @param input_v_path 输入的视频路劲
 * @param output_a_path 输入的音频路径
 * @return
 * http://blog.csdn.net/leixiaohua1020/article/details/39802913
 */
int muxer(const char* output_path , const char *input_v_path , const char *input_a_path){

    /**
     * 用于输出的结构体
     */
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx_v = NULL , *ifmt_ctx_a = NULL , *ofmt_ctx = NULL;
    AVPacket pkt ;
    int ret , i ;
    int videoindex_v = -1 , videoindex_out=-1;
    int audioindex_a = -1 , audioindex_out= -1;
    int frame_index = 0;
    int64_t  cur_pts_v = 0 , cur_pts_a = 0;
    av_register_all();
    /**
     * 打开视频输入文件
     */
    ret = avformat_open_input(&ifmt_ctx_v ,input_v_path , 0 , 0  );
    if(ret < 0 ){
        LOGE("open input video faild");
        return -1;
    }
    /**
     * 打开音频输入文件
     */
    ret = avformat_open_input(&ifmt_ctx_a ,input_a_path , 0 , 0  );
    if(ret < 0 ){
        LOGE("open input audio faild");
        return -1;
    }

    /**
     * 找到流
     */
    ret = avformat_find_stream_info(ifmt_ctx_v , 0 );
    if(ret < 0 ){
        LOGE("find video stream info faild");
        return -1;
    }
    ret = avformat_find_stream_info(ifmt_ctx_a , 0 );
    if(ret < 0 ){
        LOGE("find audio stream info faild");
        return -1;
    }

    /**
     * 为输出分配一个AVFormatContext
     */
    ret = avformat_alloc_output_context2(&ofmt_ctx , NULL , NULL , output_path);
    if(ret < 0){
        LOGE(" OPEN OUTPUT FAILD ");
        return -1;
    }

    ofmt = ofmt_ctx->oformat;
    enum AVRounding avrounding = (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    //找到视频轨道，并且将编解码复制给输出流，
    for(int i = 0 ; i < ifmt_ctx_v->nb_streams ; ++ i){
        if(ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoindex_v = i;
            AVStream *in_stream = ifmt_ctx_v->streams[i];
            AVStream *out_stream = avformat_new_stream(ofmt_ctx , in_stream->codec->codec);
            if(out_stream == NULL){
                LOGE("open output stream faild !");
                return -1;
            }
            videoindex_out = out_stream->index;

            if(avcodec_copy_context(out_stream->codec , in_stream->codec) < 0){
                LOGE(" COPY SETTING FAILD !");
                return -1;
            }
            out_stream->codec->codec_tag = 0 ;
            if(ofmt_ctx ->oformat->flags & AVFMT_GLOBALHEADER){
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }

    /**
     * 打开音频流，并且添加一个流，然后copy一个编解码到输出的音频流中。
     */
    for(int i = 0 ;i < ifmt_ctx_a->nb_streams ; ++i){
        if(ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioindex_a = i;
            AVStream *inStream = ifmt_ctx_a->streams[i];
            AVStream *outStream = avformat_new_stream(ofmt_ctx , inStream->codec->codec);
            if(outStream == NULL){
                LOGE("OEPN OUT STREAM FAILD ! ");
                return -1;
            }
            audioindex_out = outStream->index;
            if(avcodec_copy_context(outStream->codec ,inStream->codec ) < 0){
                LOGE("COPY AUDIO CODECONTEXT FAILD !");
                return -1;
            }

            outStream->codec->codec_tag = 0;
            LOGE("AUDIO FLAGS %d " ,ofmt_ctx->oformat->flags );
            if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
                outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }

    /**
     * AVFMT_NOFILE需要一个没有打开的File
     */
    if(!(ofmt->flags & AVFMT_NOFILE)){
        if(avio_open(&ofmt_ctx->pb , output_path , AVIO_FLAG_WRITE) < 0){
            LOGE("OPEN OUTPUT FILE FAILD !");
            return -1;
        }
    }

    if(avformat_write_header(ofmt_ctx , NULL) < 0){
        LOGE("write_header faild ");
        return -1;
    }

    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");

    while(1){
        AVFormatContext *ifmt_ctx;
        int stream_index = 0;
        AVStream *in_stream , *out_stream;
        //比较，是先写视频帧还是写音频帧，不然会出现音视频不同步的现象
        if(av_compare_ts(cur_pts_v,
                         ifmt_ctx_v->streams[videoindex_v]->time_base,
                         cur_pts_a,
                         ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0){
            LOGE("  write video  ");
            ifmt_ctx = ifmt_ctx_v;
            stream_index = videoindex_out;

            if(av_read_frame(ifmt_ctx , &pkt) >= 0){
                do{
                    in_stream = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];
                    if(pkt.stream_index == videoindex_v){
                        //AV_NOPTS_VALUE ，没有定义pts，dts
                        if(pkt.pts == AV_NOPTS_VALUE){
                            /**
                             * AVStream->time_base单位为秒。
                             * AVCodecContext->time_base单位同样为秒，不过精度没有AVStream->time_base高，大小为1/framerate。
                             */
                            AVRational time_base1 = in_stream->time_base;
                            //Duration between 2 frames (us) ， av_q2d-》就是将AVRational中的分子分母相除得一个小数
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters pts是显示时间 ， 当前已经有的总时间除以时间基，就是ffmpeg中的时间单位
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            //dts 解码时间
                            pkt.dts=pkt.pts;
                            //间隔时间，需要除以时间基
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_v=pkt.pts;
                        LOGE("pts %d " ,cur_pts_v );
                        break;
                    }
                }while(av_read_frame(ifmt_ctx , &pkt) >= 0);
            }
            else {
                break;
            }
        }
        else{
            LOGE("  write audio  ");
            ifmt_ctx=ifmt_ctx_a;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >= 0){
                do{
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = ofmt_ctx->streams[stream_index];

                    if(pkt.stream_index==audioindex_a){

                        //FIX：No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >= 0);
            }else{
                break;
            }
        }
        av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
        av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
        //Convert PTS/DTS
        // equivalent to `a * bq / cq`.,time_base 间的转换
        /**
         * 从一种容器中demux出来的源AVStream的frame，存入另一个容器中某个目的AVStream。
            此时的时间刻度应该从源AVStream的time，转换成目的AVStream timebase下的时间。
            -----------------------
            其实，问题的关键还是要理解，不同的场景下取到的数据帧的time是相对哪个时间体系的。
            demux出来的帧的time：是相对于源AVStream的timebase
            编码器出来的帧的time：是相对于源AVCodecContext的timebase
            mux存入文件等容器的time：是相对于目的AVStream的timebase
            这里的time指pts。
         */
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, avrounding);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, avrounding);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

        printf("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
            printf( "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);
    }

    LOGE("MUXER END ....");
    return 0;
}