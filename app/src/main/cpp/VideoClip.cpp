//
// http://ffmpeg.org/doxygen/3.4/remuxing_8c-example.html#a25 可以参考
//

/**
 * 现在改到任意帧，也就是需要解码视频帧，不然会有花屏的情况。因为第一帧不是关键帧。
 * 音频帧就不用解码了。直接写入。
 */
#include "VideoClip.h"


VideoClip::VideoClip(const char *path, const char *output, int startSecond, int endSecond) {
    this->startSecond = startSecond;
    this->endSecond = endSecond;

    int pathLen = strlen(path);
    pathLen++;
    this->path = (char *) malloc(pathLen);
    strcpy(this->path, path);

    int outputPathLen = strlen(output);
    outputPathLen++;
    this->outputPath = (char *) malloc(outputPathLen);
    strcpy(this->outputPath, output);

}

int VideoClip::initInput() {
    int result = 0;
    afc_input = NULL;
    result = avformat_open_input(&afc_input, path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input FAILD !");
        return -1;
    }
    result = avformat_find_stream_info(afc_input, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return -1;
    }

    for (int i = 0; i < afc_input->nb_streams; ++i) {
        AVStream *stream = afc_input->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = stream;
            video_index = i;
            videoCodecD = avcodec_find_decoder(stream->codecpar->codec_id);
            width = videoStream->codecpar->width;
            height = videoStream->codecpar->height;
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream = stream;
            audio_index = i;
        }
    }

    if (videoCodecD == NULL) {
        LOGE(" 没找到视频解码器 ");
        return -1;
    }
    vCtxD = avcodec_alloc_context3(videoCodecD);

    if (!vCtxD) {
        LOGE("vc AVCodecContext FAILD ! ");
        return -1;
    }

    avcodec_parameters_to_context(vCtxD, afc_input->streams[video_index]->codecpar);

    result = avcodec_open2(vCtxD, videoCodecD, NULL);

    if (result < 0) {
        LOGE(" decode avcodec_open2 Faild !");
        return -1;
    }
    LOGE(" init input success ");

    return 1;
}

int VideoClip::initOutput() {
    int result = 0;
    afc_output = NULL;
    result = avformat_alloc_output_context2(&afc_output, NULL, NULL, outputPath);
    if (result < 0 || afc_output == NULL) {
        LOGE(" avformat_alloc_output_context2 faild %s ", av_err2str(result));
        return -1;
    }
    afot = afc_output->oformat;
    if (addVideoOutputStream(width, height) < 0) {
        return -1;
    }
    if (addAudioOutputStream() < 0) {
        return -1;
    }

    if (!(afot->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc_output->pb, outputPath, AVIO_FLAG_WRITE);
        if (result < 0) {
            LOGE("Could not open output file %s ", outputPath);
            return -1;
        }
    }

    result = avformat_write_header(afc_output, NULL);

    if (result < 0) {
        LOGE(" avformat_write_header %s", av_err2str(result));
    }

    LOGE(" INIT OUTPUT SUCCESS !");

    return 1;
}


int VideoClip::addVideoOutputStream(int width, int height) {
    int result = 0;
    videoOutStream = avformat_new_stream(afc_output, NULL);
    if (videoOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }

    if (afot->video_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }

    videoCodecE = avcodec_find_encoder(afot->video_codec);

    if (videoCodecE == NULL) {
        LOGE(" avcodec_find_encoder FAILD ! ");
        return -1;
    }
    vCtxE = avcodec_alloc_context3(videoCodecE);

    if (vCtxE == NULL) {
        LOGE(" avcodec_alloc_context3 FAILD ! ");
        return -1;
    }

    vCtxE->bit_rate = 400000;
    vCtxE->time_base = (AVRational) {1, 25};
    vCtxE->framerate = (AVRational) {25, 1};
    vCtxE->gop_size = 10;
    vCtxE->max_b_frames = 1;
    vCtxE->pix_fmt = AV_PIX_FMT_YUV420P;
    vCtxE->codec_type = AVMEDIA_TYPE_VIDEO;
    vCtxE->width = width;
    vCtxE->height = height;

    result = avcodec_parameters_from_context(videoOutStream->codecpar, vCtxE);

    if (result < 0) {
        LOGE(" avcodec_parameters_from_context FAILD ! ");
        return -1;
    }

    result = avcodec_open2(vCtxE, videoCodecE, NULL);

    if (result < 0) {
        LOGE("Could not open codec %s ", av_err2str(result));
        return -1;
    }

    return 1;
}

int VideoClip::addAudioOutputStream() {
    int result = 0;
    audioOutStream = avformat_new_stream(afc_output, NULL);
    if (audioOutStream == NULL) {
        LOGE(" VIDEO STREAM NULL ");
        return -1;
    }
    if (afot->audio_codec == AV_CODEC_ID_NONE) {
        LOGE(" VIDEO AV_CODEC_ID_NONE ");
        return -1;
    }
    avcodec_parameters_copy(audioOutStream->codecpar, audioStream->codecpar);
    audioOutStream->codecpar->codec_tag = 0;

    return 1;
}





AVFrame *VideoClip::deocdePacket(AVPacket *packet) {

    int result = avcodec_send_packet(vCtxD, packet);
    if (result < 0) {
        LOGE("  avcodec_send_packet %s ", av_err2str(result));
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    while (result >= 0) {
        result = avcodec_receive_frame(vCtxD, frame);
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


AVPacket *VideoClip::encodeFrame(AVFrame *frame) {
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



void VideoClip::write_frame(AVStream *inStream, AVStream *outStream, AVPacket *packet) {
    packet->pts = av_rescale_q_rnd(packet->pts, inStream->time_base, outStream->time_base,
                                   AV_ROUND_NEAR_INF);
    packet->dts = av_rescale_q_rnd(packet->dts, inStream->time_base, outStream->time_base,
                                   AV_ROUND_NEAR_INF);
    packet->duration = av_rescale_q(packet->duration, inStream->time_base, outStream->time_base);
    av_interleaved_write_frame(afc_output, packet);
}


void VideoClip::startClip() {

    int result = -1;
    av_register_all();
    avcodec_register_all();
#ifdef DEBUG
//    av_log_set_callback(custom_log);
    LOGE(" dubug ");
#endif

    if (initInput() < 0) {
        LOGE(" INIT INPUT FAILD ");
        return;
    }
    if (initOutput() < 0) {
        LOGE(" INIT OUTPUT FAILD ");
        return;
    }


    //这里需要直接seek到裁剪的开始时间，节约时间。然后直接开始解码。就不会存在花屏问题。
    result = av_seek_frame(afc_input, -1,
                           ((float) startSecond / 1000) * AV_TIME_BASE * afc_input->start_time,
                           AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE(" SEEK FRAME FAILD ! %s ", av_err2str(result));
        return;
    }
    int64_t pts = 0;
    while (true) {
        AVPacket *packet = av_packet_alloc();
        result = av_read_frame(afc_input, packet);
        if (result < 0) {
            LOGE(" ********************** READ FRAME FAILD *********************");
            av_packet_free(&packet);
            break;
        }
        if (packet->stream_index == video_index) {
            AVFrame *frame = deocdePacket(packet);
            if (frame != NULL) {
                pts = (int64_t) (frame->pts * av_q2d(videoStream->time_base) * 1000);
                if ((pts >= startSecond * 1000) && (pts <= endSecond * 1000)) {
                    AVPacket *newPkt = encodeFrame(frame);
                    if (newPkt != NULL) {
                        LOGE("WRITE VIDEO ");
                        write_frame(videoStream, videoOutStream, newPkt);
                        av_packet_free(&newPkt);
                    }
                }
                av_frame_free(&frame);
            }
            av_packet_free(&packet);
        } else if (packet->stream_index == audio_index) {
            pts = (int64_t) (packet->pts * av_q2d(audioStream->time_base) * 1000);
            if ((pts >= startSecond * 1000) && (pts <= endSecond * 1000)) {
                LOGE(" WRITE AUDIO ");
                write_frame(audioStream, audioOutStream, packet);
            }
            av_packet_free(&packet);
        }
        if (pts > endSecond * 1000) {
            LOGE(" ************* READ ENOUGH ************* ");
            break;
        }
    }
    av_write_trailer(afc_output);

}


VideoClip::~VideoClip() {


    if(vCtxE != NULL){
        avcodec_free_context(&vCtxE);
    }
    if(vCtxD != NULL){
        avcodec_free_context(&vCtxD);
    }
    if (afc_input != NULL) {
        avformat_close_input(&afc_input);
    }
    if (afc_output != NULL) {
        avformat_free_context(afc_output);
    }
    if(path != NULL){
        free(path);
    }
    if(outputPath != NULL){
        free(outputPath);
    }

}