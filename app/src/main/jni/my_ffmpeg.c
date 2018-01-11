#include "my_ffmpeg.h"
#include "My_LOG.h"
#include <string.h>
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
#include <libavutil/mathematics.h>

/**
 * 将相机的yuv数据放进来编码成MP4（h264格式）
 */
const char *outPath;
int width , height;
AVFormatContext *pOFC;
AVOutputFormat *oft;
AVStream *video_st;
int framecnt = 0;
AVCodec *pCodec;
AVFrame *pFrame;
int pic_size;
uint8_t *picture_buf;
AVPacket *pkt;
int y_size;
FILE *oFile;


int init(const char *ouputPath , int w , int h){
    av_register_all();
    av_log_set_callback(custom_log);
    char *outYuv = "/storage/emulated/0/FFmpeg/my_camera.yuv";
    oFile = fopen( outYuv , "wb+");
    int ret = 0;
    outPath = ouputPath;
    LOGE(" INIT ...%s width = %d , height = %d" , outPath  , w , h );
    width = w;
    height = h;
    oft = av_guess_format(NULL, ouputPath, NULL);
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
    ret = avio_open(&pOFC->pb, outPath, AVIO_FLAG_READ_WRITE);
    if (ret < 0) {
        LOGE(" avio_open faild! ");
        return -1;
    }
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
    //设置图像组的大小，表示两个i帧之间的间隔
    video_st->codec->gop_size = 100;
    video_st->codec->time_base.num = 1;
    video_st->codec->time_base.den = 25;
    //最小视频量化标度，设定最小质量。
    video_st->codec->qmin = 30;
    video_st->codec->qmax = 51;

    pCodec = avcodec_find_encoder(video_st->codec->codec_id);

    if (pCodec == NULL) {
        LOGE("  pCodec null ");
        return -1;
    }

    if (avcodec_open2(video_st->codec, pCodec, NULL) < 0) {
        LOGE("Failed to open encoder! \n");
        return -1;
    }

    pFrame = av_frame_alloc();

    pic_size = avpicture_get_size(video_st->codec->pix_fmt, video_st->codec->width,
                                      video_st->codec->height);

    pFrame->format = video_st->codec->pix_fmt;
    pFrame->width  = video_st->codec->width;
    pFrame->height = video_st->codec->height;
    LOGE(" pic_size %d ", pic_size);

    picture_buf = (uint8_t *) av_malloc(pic_size);

    /**
     * Setup the data pointers and linesizes based on the specified image parameters and the provided array.
     */
    avpicture_fill((AVPicture *) pFrame, picture_buf, video_st->codec->pix_fmt,
                   video_st->codec->width, video_st->codec->height);

    pFrame->format = video_st->codec->pix_fmt;
    pFrame->width  = video_st->codec->width;
    pFrame->height = video_st->codec->height;

    avformat_write_header(pOFC, NULL);
    pkt = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_new_packet(pkt, pic_size);
    y_size = video_st->codec->width * video_st->codec->height;
    LOGE(" INIT SUCCESS ...");
    return ret;
}



int encodeCamera(jbyte *navtiveYuv){

    if(navtiveYuv == NULL){
        return -1;
    }
    LOGE(" ENCODE..");
    fwrite(navtiveYuv , 1 , (width * height) * 3 / 2 , oFile);
    return 1;
    //注意反调下，vu分量。不然有红绿相对调的情况出现。
    /**
     * http://ffmpeg.org/doxygen/3.2/decoding_encoding_8c-example.html#a59
     * 关于pFrame->data[0]的使用，不是很明白
     * 如果是p模式就是几个分量分开存
     * 如果是package模式就全部放入data【0】中，但是要看编码器支持不pix_fmt,目前video就发现只支持420p的，所以我的锤子手机需要自己转格式。
     */

    pFrame->data[0] = (uint8_t *)navtiveYuv;
    pFrame->data[1] =(uint8_t *) navtiveYuv + y_size * 5 / 4;
    pFrame->data[2] = (uint8_t *) navtiveYuv + y_size;

    pFrame->pts = framecnt * (video_st->time_base.den) / ((video_st->time_base.num) * 25);

    int got_picture = 0;

    int ret = avcodec_encode_video2(video_st->codec, pkt, pFrame, &got_picture);

    if (ret < 0) {
        LOGE(" FAILD ENCODE ");
        return -1;
    }

    if (got_picture == 1) {
        LOGE(" ENCODE success %d", framecnt);
//        framecnt++;
        pkt->stream_index = video_st->index;
        ret = av_write_frame(pOFC, pkt);
        av_free_packet(pkt);
    }
//    LOGE(" encodeCamera ...%d " , sizeof(navtiveYuv));
    framecnt++ ;
    return 0;
}

int close(){
    LOGE(" CLOSE ...");
    av_write_trailer(pOFC);
    fclose(oFile);
    //Clean
    if (video_st) {
        avcodec_close(video_st->codec);
        av_free(pFrame);
        av_free(picture_buf);
    }
    avio_close(pOFC->pb);
    avformat_free_context(pOFC);

    return 0;
}

//此函数在锤子手机m1上测试成功
void nv21ToYv12(jbyte *navtiveYuv){

    if(navtiveYuv){
        int length = y_size * 3 / 2;
        jbyte *result = malloc(sizeof(jbyte) * length);
        memset(result ,0 , sizeof(jbyte) *  length);
        memcpy(result , navtiveYuv , y_size);
        int uCount = (length - y_size) / 2;
        LOGE(" ucount %d  , length %d " ,uCount , length);
        for(int i = y_size , uPosition = (y_size+uCount) , vPosition = y_size ;i < length ; i+= 2 , uPosition++ , vPosition++){
            result[vPosition] =  navtiveYuv[i + 1]; //u
            result[uPosition] = navtiveYuv[i]; //v
        }
        memcpy(navtiveYuv, result  , length);
        free(result);
    }
}