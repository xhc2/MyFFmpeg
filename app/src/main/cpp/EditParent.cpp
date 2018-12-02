//
// Created by Administrator on 2018/10/25/025.
//

#include <my_log.h>
#include "EditParent.h"

void EditParent::custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeg_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};

EditParent::EditParent() {
    av_register_all();
#ifdef DEBUG
    av_log_set_callback(custom_log);
#endif
    videoStreamIndex = -1;
    audioStreamIndex = -1;
    videoOutputStreamIndex = -1;
    audioOutputStreamIndex = -1;
}

int EditParent::open_input_file(const char *filename, AVFormatContext **fmt_ctx) {
    int ret;
    if ((ret = avformat_open_input(fmt_ctx, filename, NULL, NULL)) < 0) {
        LOGE("avformat_open_input FAILD !");
        return ret;
    }
    if(fmt_ctx == NULL){
        LOGE("alloc fmt_ctx  FAILD !");
        return -1;
    }
    if ((ret = avformat_find_stream_info(*fmt_ctx, NULL)) < 0) {
        LOGE("avformat_find_stream_info FAILD !");
        return ret;
    }
    return 1;
}

int EditParent::getVideoDecodeContext(AVFormatContext *fmt_ctx, AVCodecContext **dec_ctx) {
    int ret;
    AVCodec *dec = NULL;
    /* select the video stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        LOGE("av_find_best_stream FAILD !");
        return ret;
    }
    videoStreamIndex = ret;
    /* create decoding context */
    *dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(*dec_ctx, fmt_ctx->streams[videoStreamIndex]->codecpar);
    /* init the video decoder */
    if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
        LOGE("avcodec_open2 FAILD !");
        return ret;
    }
    return videoStreamIndex;
}

AVFrame* EditParent::decodePacket(AVCodecContext *decode , AVPacket *packet){
    int result = avcodec_send_packet(decode, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(decode, frame);
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


AVPacket *EditParent::encodeFrame(AVFrame *frame ,AVCodecContext *vCtxE ) {
    int result = 0;
    result = avcodec_send_frame(vCtxE, frame);
    if (result < 0) {
        LOGE(" avcodec_send_frame faild ! %s ", av_err2str(result));
        return NULL;
    }
    AVPacket *packet = av_packet_alloc();
    while (result >= 0) {
        result = avcodec_receive_packet(vCtxE, packet);
        if (result < 0) {
            LOGE(" avcodec_receive_packet faild ! %s ", av_err2str(result));
            av_packet_free(&packet);
            return NULL;
        }
        return packet;
    }
    return NULL;
}
int EditParent::getVideoStreamIndex(AVFormatContext *fmt_ctx){
    if(videoStreamIndex == -1){
        videoStreamIndex = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    }
    return videoStreamIndex ;
}
int EditParent::getAudioStreamIndex(AVFormatContext *fmt_ctx){
    if(audioStreamIndex == -1){
        audioStreamIndex = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    }
    return audioStreamIndex;
}


int EditParent::initOutput(const char* ouput ,AVFormatContext **ctx){
    int result = 0;
    result = avformat_alloc_output_context2(ctx, NULL, NULL, ouput);
    if (result < 0 || ctx == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    return 1;
}

int EditParent::addOutputVideoStream(AVFormatContext *afc_output ,AVCodecContext **vCtxE , int outW , int outH ){
    int result = 0;
    AVStream *videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    AVOutputFormat *afot = afc_output->oformat;
    videoOutputStreamIndex = videoOutStream->index;
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    AVCodec *videoCodecE = avcodec_find_encoder(afot->video_codec);
    if (videoCodecE == NULL) {
        LOGE("VIDEO avcodec_find_encoder FAILD ! ");
        return -1;
    }
    LOGE("video ENCODE NAME %s ", videoCodecE->name);
    *vCtxE = avcodec_alloc_context3(videoCodecE);

    if (*vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }
    int outFrameRate = 25;
    (*vCtxE)->bit_rate = 400000;
    (*vCtxE)->time_base = (AVRational) {1, outFrameRate};
    (*vCtxE)->framerate = (AVRational) {outFrameRate, 1};
    (*vCtxE)->gop_size = 10;
//    vCtxE->max_b_frames = 1;
    (*vCtxE)->pix_fmt = AV_PIX_FMT_YUV420P;
    (*vCtxE)->codec_type = AVMEDIA_TYPE_VIDEO;
    (*vCtxE)->width = outW;
    (*vCtxE)->height = outH;

    result = avcodec_parameters_from_context(videoOutStream->codecpar, *vCtxE);

    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }

    result = avcodec_open2(*vCtxE, videoCodecE, NULL);

    if (result < 0) {
        LOGE("video Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS VIDEO !");
    return videoOutputStreamIndex;
}
int EditParent::addOutputAudioStream(AVFormatContext *afc_output ,AVCodecContext **aCtxE , int sampleRate , uint64_t outChannelLayout){
    int result = 0;
    AVStream *audioOutStream = avformat_new_stream(afc_output, NULL);
    if (audioOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    AVOutputFormat *afot = afc_output->oformat;
    audioOutputStreamIndex = audioOutStream->index;
    if (afot->audio_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }

    LOGE(" afot->audio_codec   %d ", afot->audio_codec);

    AVCodec *audioCodecE = avcodec_find_encoder(afot->audio_codec);
    if (audioCodecE == NULL) {
        LOGE(" audioCodecE NULL ");
        return -1;
    }
    LOGE("AUDIO CODEC NAME %s ", audioCodecE->name);
    *aCtxE = avcodec_alloc_context3(audioCodecE);

    if ( *aCtxE == NULL) {
        LOGE("AUDIO avcodec_alloc_context3 FAILD !");
        return -1;
    }
    int channel = av_get_channel_layout_nb_channels(outChannelLayout);
    AVSampleFormat sampleFormat = AV_SAMPLE_FMT_FLTP;
    (*aCtxE)->bit_rate = 64000;
    (*aCtxE)->sample_fmt = sampleFormat;
    (*aCtxE)->sample_rate = sampleRate;
    (*aCtxE)->channel_layout = outChannelLayout;
    (*aCtxE)->channels = channel;
    (*aCtxE)->time_base = (AVRational) {1, sampleRate};
    (*aCtxE)->codec_type = AVMEDIA_TYPE_AUDIO;
    audioOutStream->time_base = (*aCtxE)->time_base;

    result = avcodec_parameters_from_context(audioOutStream->codecpar, *aCtxE);
    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }
    result = avcodec_open2( *aCtxE, audioCodecE, NULL);
    if (result < 0) {
        LOGE(" audio Could not open codec %s ", av_err2str(result));
        return -1;
    }

    LOGE(" INIT OUTPUT SUCCESS AUDIO   ");
    return audioOutputStreamIndex;
}

int EditParent::getVideoOutputStreamIndex(){

    return videoOutputStreamIndex;
}
int EditParent::getAudioOutputStreamIndex(){
    return audioOutputStreamIndex;
}

int EditParent::writeOutoutHeader(AVFormatContext *afc_output , const char* outputPath){
    int result ;
    if (!(afc_output->oformat->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc_output->pb, outputPath, AVIO_FLAG_WRITE);
        if (result < 0) {
            LOGE("Could not open output file %s ", outputPath);
            return -1;
        }
    }
    result = avformat_write_header(afc_output, NULL);

    if (result < 0) {
        LOGE(" avformat_write_header %s", av_err2str(result));
        return -1;
    }
    return 1;
}

int  EditParent::writeTrail(AVFormatContext *afc_output ){
    av_write_trailer(afc_output);
    return 1;
}

EditParent::~EditParent() {

}