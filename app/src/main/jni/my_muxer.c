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

    ret = avformat_open_input(&ifmt_ctx  ,input_path , 0 , 0);
    if(ret < 0){
        LOGE("CANT OPEN INPUT !");
        return -1;
    }
    ret = avformat_find_stream_info(ifmt_ctx , 0);
    if(ret < 0){
        LOGE("avformat_find_stream_info faild !");
        return -1;
    }
    avformat_alloc_output_context2(&ofmt_ctx_v , NULL , NULL , out_v_path);
    if(ofmt_ctx_v == NULL){
        LOGE("avformat_alloc_output_context2 V faild !");
        return -1;
    }
    ofmt_v =ofmt_ctx_v->oformat;

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

        //这个不太明白什么意思
        if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
            outStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }

//    av_dump_format(ifmt_ctx , 0 , input_path , 0);
//    av_dump_format(ofmt_ctx_v , 0 , out_v_path , 1);
//    av_dump_format(ofmt_ctx_a , 0 , out_a_path , 1);

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
    enum AVRounding AVROUNDING = AV_ROUND_NEAR_INF , AV_ROUND_PASS_MINMAX ;

    while(1){

        AVOutputFormat *ofmt_ctx;
        AVStream *inStream , *outStream;
        if(av_read_frame(ifmt_ctx , &pkt) < 0){
            LOGE(" READ FRAME < 0 ");
            break;
        }
        inStream = ifmt_ctx->streams[pkt.stream_index];
        if(pkt.stream_index == videoindex){
            outStream = ofmt_ctx_v->streams[0];
            ofmt_ctx = ofmt_ctx_v;
            av_bitstream_filter_filter(h264bsfc, inStream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
            LOGE(" video ...");
        }

        else if(pkt.stream_index == audioindex){
            outStream = ofmt_ctx_a->streams[0];
            ofmt_ctx = ofmt_ctx_a;
            LOGE(" audio ...");
        }
        else{
            LOGE(" CONTINUE ...");
            continue;
        }

        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, inStream->time_base, outStream->time_base, AVROUNDING);
        pkt.dts = av_rescale_q_rnd(pkt.dts, inStream->time_base, outStream->time_base, AVROUNDING);
        pkt.duration = av_rescale_q(pkt.duration, inStream->time_base, outStream->time_base);
        pkt.pos = -1;
        pkt.stream_index=0;

        if(av_interleaved_write_frame(ofmt_ctx , &pkt) < 0){
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
 */
int muxer(const char* output_path , const char *input_v_path , const char *input_a_path){



    return 0;
}