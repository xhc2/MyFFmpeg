#include <malloc.h>
#include <CallJava.h>
#include "CameraStream.h"
#include <my_log.h>

/**
 * 将视频，和声音录制了，然后再编码处理。
 */
CameraStream::CameraStream(const char *url, int width, int height, int pcmsize, CallJava *cj) {
    afc = NULL;
    afot = NULL;
    yuv = NULL;
    pcm = NULL;
    count = 0;
    pixFmt = AV_PIX_FMT_YUV420P;
    this->url = url;
    this->width = width;
    this->height = height;
    this->outWidth = 640;
    this->outHeight = 360;
    this->nbSample = 0;
    videoIndex = -1;
    audioIndex = -1;
    vpts = 0;
    apts = 0;
//    filePCM = fopen("sdcard/FFmpeg/pcm.pcm" , "wb+");
    timeBaseFFmpeg = (AVRational){ 1, AV_TIME_BASE };
    sws = sws_getContext(width, height, pixFmt, outWidth, outHeight,
                         pixFmt, SWS_BILINEAR, NULL, NULL, NULL);
    if (sws == NULL) {
        cj->callStr(" sws_getContext FAILD2 !");
        return;
    }
    LOGE(" path %s , width = %d , height = %d ", url, width, height);
    this->size = (int) (width * height * 1.5f);
    this->cj = cj;
    yuv = (char *) malloc(size * sizeof(char));

    if (yuv == NULL) {
        cj->callStr("YUV ALLOC FAILD2 !");
        return;
    }
    pcm = (char *) malloc(pcmsize * sizeof(char));
    if (pcm == NULL) {
        cj->callStr("YUV ALLOC FAILD2 !");
        return;
    }
    initFFmpeg();
//    this->start();
//    this->setPause();
}

void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeglog.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};


void CameraStream::initFFmpeg() {
    int result = 0;
    av_register_all();
    avformat_network_init();
    av_log_set_callback(custom_log);
    result = avformat_alloc_output_context2(&afc, NULL, "flv", url);
    if (result < 0 || afc == NULL) {
        cj->callStr(" avformat_alloc_output_context2 faild ");
        return;
    }
    afot = afc->oformat;

//    addVideoStream();
    addAudioStream();

    if (!(afc->flags & AVFMT_NOFILE)) {
        result = avio_open(&afc->pb, url, AVIO_FLAG_WRITE);
        if (result < 0) {
            cj->callStr("Could not open output file  ");
            return;
        }
    }

    result = avformat_write_header(afc, NULL);
    if (result < 0) {
        cj->callStr(" avformat_write_header faild ! ");
        return;
    }
    LOGE(" FFMPEG SUCCESS ! ");
}

void CameraStream::addVideoStream() {
    int result = 0;
    videoOS = avformat_new_stream(afc, NULL);
    if (videoOS == NULL) {
        cj->callStr("CREATE NEW STREAM FAILD ");
        return;
    }
    if (afot->video_codec == AV_CODEC_ID_NONE) {
        cj->callStr(" VIDEO AV_CODEC_ID_NONE ");
        return;
    }

    AVCodec *vCode = avcodec_find_encoder(afot->video_codec);

    if (vCode == NULL) {
        cj->callStr(" avcodec_find_video_encoder faild ! ");
        return;
    }

    vCodeCtx = avcodec_alloc_context3(vCode);
    if (vCodeCtx == NULL) {
        cj->callStr(" vcode context faild ! ");
        return;
    }
    vCodeCtx->width = outWidth;
    vCodeCtx->height = outHeight;
    vCodeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    vCodeCtx->gop_size = 20;
    vCodeCtx->pix_fmt = pixFmt;
    vCodeCtx->codec_id = afot->video_codec;
    vCodeCtx->bit_rate = 400000;
    vCodeCtx->time_base = (AVRational) {1, 25};
    vCodeCtx->framerate = (AVRational) {25, 1};
    vCalDuration = (double)(AV_TIME_BASE)*(1 / av_q2d(vCodeCtx->framerate));

    LOGE(" video frame duration %lld " , vCalDuration );
    vCodeCtx->thread_count = 4;
    videoOS->codec = vCodeCtx;
    result = avcodec_parameters_from_context(videoOS->codecpar, vCodeCtx);
    if (result < 0) {
        cj->callStr(" avcodec_parameters_from_context faild ");
        return;
    }

    result = avcodec_open2(vCodeCtx, NULL, NULL);

    if (result < 0) {
        cj->callStr("avcodec_open2 faild ! ");
        return;
    }
    framePic = av_frame_alloc();
    framePic->format = pixFmt;
    framePic->width = width;
    framePic->height = height;
    result = av_frame_get_buffer(framePic, 0);
    if (result < 0) {
        cj->callStr("av_frame_get_buffer faild ! ");
        return;
    }
    result = av_frame_make_writable(framePic);
    if (result < 0) {
        cj->callStr("av_frame_make_writable faild ! ");
        return;
    }

    outFrame = av_frame_alloc();
    outFrame->format = pixFmt;
    outFrame->width = outWidth;
    outFrame->height = outHeight;
    result = av_frame_get_buffer(outFrame, 0);
    if (result < 0) {
        cj->callStr("av_frame_get_buffer faild ! ");
        return;
    }

    result = av_frame_make_writable(outFrame);
    if (result < 0) {
        cj->callStr("av_frame_make_writable faild ! ");
        return;
    }

    if (afc->oformat->flags & AVFMT_GLOBALHEADER)
        vCodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    videoIndex = videoOS->index;
    LOGE(" videoIndex index %d ", videoIndex);
}

void CameraStream::addAudioStream() {
    int result = 0;
    audioOS = avformat_new_stream(afc, NULL);
    if (audioOS == NULL) {
        cj->callStr("CREATE NEW AUDIO STREAM FAILD ");
        return;
    }
    if (afot->audio_codec == AV_CODEC_ID_NONE) {
        cj->callStr(" AUDIO AV_CODEC_ID_NONE ");
        return;
    }
    AVCodec *vCode = avcodec_find_encoder(afot->audio_codec);

    if (vCode == NULL) {
        cj->callStr(" avcodec_find_video_encoder faild ! ");
        return;
    }

    aCodeCtx = avcodec_alloc_context3(vCode);
    if (aCodeCtx == NULL) {
        cj->callStr(" aCodeCtx context faild ! ");
        return;
    }

    AVSampleFormat audioFormat = AV_SAMPLE_FMT_S16;
    int sampleRate = 44100;
    int channels = av_get_channel_layout_nb_channels(1);
    int64_t channelLayout = av_get_default_channel_layout(1);
    aCodeCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    aCodeCtx->sample_fmt = audioFormat;
    aCodeCtx->bit_rate = 32000;
    aCodeCtx->sample_rate = sampleRate;
    aCodeCtx->channels = channels;
    aCodeCtx->channel_layout = (uint64_t) channelLayout;
    aCodeCtx->time_base =  (AVRational) {1, aCodeCtx->sample_rate};

    audioOS->time_base = (AVRational) {1, aCodeCtx->sample_rate};
    audioOS->codecpar->codec_id = afot->audio_codec;
    audioOS->codec = aCodeCtx;

    //两个采样之间的间隙
    AVRational sampleRateAv = (AVRational){ sampleRate , 1 };
    aCalDuration = (double)(AV_TIME_BASE)*(1 / av_q2d(sampleRateAv));
    LOGE("  aCalDuration %lld " , aCalDuration);
    result = avcodec_parameters_from_context(audioOS->codecpar, aCodeCtx);
    if (result < 0) {
        cj->callStr(" audioOS avcodec_parameters_from_context faild ");
        return;
    }

    result = avcodec_open2(aCodeCtx, NULL, NULL);

    if (result < 0) {
        cj->callStr("audioOS avcodec_open2 faild ! ");
        return;
    }


    frameAudio = av_frame_alloc();
    frameAudio->format = audioFormat;
    frameAudio->sample_rate = sampleRate;
    frameAudio->channel_layout = (uint64_t) channelLayout;
    frameAudio->channels = channels;
    frameAudio->nb_samples = aCodeCtx->frame_size ;

    LOGE(" FRAME SIZE %d ", aCodeCtx->frame_size);

    result = av_frame_get_buffer(frameAudio, 0);
    if (result < 0) {
        LOGE(" AUDIO FRAME GET BUFFER FAILD %s ", av_err2str(result));
        cj->callStr("frameAudio av_frame_get_buffer faild ! ");
        return;
    }
    result = av_frame_make_writable(frameAudio);
    if (result < 0) {
        cj->callStr("frameAudio av_frame_make_writable faild ! ");
        return;
    }

    if (afc->oformat->flags & AVFMT_GLOBALHEADER)
        aCodeCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    audioIndex = audioOS->index;

    LOGE(" audioIndex index %d ", audioIndex);
}

void CameraStream::encodeVideoFrame() {

    //y
    framePic->data[0] = (uint8_t *) (this->yuv);
    //u
    framePic->data[1] = (uint8_t *) (this->yuv + width * height * 5 / 4);
    //v
    framePic->data[2] = (uint8_t *) (this->yuv + width * height);
    //修改分辨率统一输出大小
    sws_scale(sws, (const uint8_t *const *) framePic->data, framePic->linesize,
              0, height, outFrame->data, outFrame->linesize);
//    //os->time_base 就是将一秒钟分成了多少份，看帧率是多少。然后看一帧占多少份。
    vpts = av_rescale_q(count * vCalDuration, timeBaseFFmpeg, videoOS->time_base);//count * (videoOS->time_base.den) / ((videoOS->time_base.num) * 25)  ;
    LOGE(" SET VIDEO PTS %lld " , vpts );
    outFrame->pts = vpts;
//
    count++;
    int ret = avcodec_send_frame(vCodeCtx, outFrame);
    if (ret < 0) {
        LOGE(" Error sending a frame for encoding ");
        return;
    }
    while (ret >= 0) {
        AVPacket *pkt = av_packet_alloc();
        ret = avcodec_receive_packet(vCodeCtx, pkt);
        pkt->stream_index = videoIndex;
        if (ret < 0) {
            LOGE("avcodec_receive_packet FAILD %s ", av_err2str(ret));
            av_packet_free(&pkt);
            return;
        }
        LOGE(" RECIVE VIDEO  PTS %lld " , pkt->pts );
        MyData *myData = new MyData();
        myData->pkt = pkt;
        myData->isAudio = false;
        myData->size = pkt->size;
        videoPktQue.push(myData);
    }
}

void CameraStream::encodeAudioFrame(int pcmSize) {

    frameAudio->data[0] = (uint8_t *) this->pcm;

    apts += 1.0f * pcmSize / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) / audioOS->time_base.den * 1000;
//    apts = pcmSize / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) ;

    nbSample += pcmSize / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    LOGE(" NBSAMPLE %d " , nbSample);
//    apts = av_rescale_q(nbSample * aCalDuration, timeBaseFFmpeg, audioOS->time_base);

    frameAudio->pts = apts;

    int ret = avcodec_send_frame(aCodeCtx, frameAudio);
    if (ret < 0) {
        LOGE(" Error sending a frame for encoding ");
        return;
    }
    while (ret >= 0) {
        AVPacket *pkt = av_packet_alloc();
        ret = avcodec_receive_packet(aCodeCtx, pkt);
        pkt->stream_index = audioIndex;
        if (ret < 0) {
//            LOGE("avcodec_receive_packet FAILD %s ", av_err2str(ret));
            av_packet_free(&pkt);
            return;
        }
        av_write_frame(afc , pkt);
        av_packet_free(&pkt);
//        MyData *myData = new MyData();
//        myData->isAudio = true;
//        myData->size = pcmSize;
//        myData->pkt = pkt;
//        audioPktQue.push(myData);
    }
}


void CameraStream::writeVideoPacket() {
//    LOGE(" VIDEO QUE %d " , videoPktQue.size());
    if (videoPktQue.size() > 0) {
        MyData *myData = videoPktQue.front();
        AVPacket *pkt = myData->pkt;
        videoPktQue.pop();
        pkt->stream_index = videoIndex;
        wvpts = pkt->pts;
        LOGE(" WRITE **VIDEO PTS %lld ", wvpts);
        int result;

        if ((result = av_interleaved_write_frame(afc, pkt)) < 0) {
            LOGE("writeVideoPacket FAILD %s ", av_err2str(result));
        };
        myData->drop();
    }
}


void CameraStream::writeAudioPacket() {
//    LOGE(" AUDIO QUE %d " , audioPktQue.size());
    if (audioPktQue.size() > 0) {
        MyData *myData = audioPktQue.front();
        AVPacket *pkt = myData->pkt;
        audioPktQue.pop();
        if(pkt == NULL){
            LOGE(" AUDIO PKT NULL ");
            myData->drop();
            return ;
        }

        pkt->stream_index = audioIndex;
        wapts = pkt->pts;
        LOGE(" WRITE AUDIO PTS %lld ", wapts);
        int result;



        av_write_frame(afc, pkt);
//        if ((result = av_interleaved_write_frame(afc, pkt)) < 0) {
//            LOGE("writeAudioPacket FAILD %s ", av_err2str(result));
//        };
        myData->drop();
    }
}


void CameraStream::startRecord() {
    pause = false;
}

void CameraStream::pauseRecord() {
    pause = true;
}

void CameraStream::pushAudioStream(jbyte *pcm, int size) {
    if (pause) {
        return;
    }
    this->pcmSize = size;
    memcpy(this->pcm, pcm, pcmSize);
    encodeAudioFrame(this->pcmSize);
}

void CameraStream::pushVideoStream(jbyte *yuv) {
    if (pause) {
        return;
    }
    memcpy(this->yuv, yuv, size);
    encodeVideoFrame();
}


//新开一个线程来混合
void CameraStream::run() {
    while (!isExit) {
        if (pause) {
            threadSleep(3);
            continue;
        }
//        writeVideoPacket();
        writeAudioPacket();
//        if (av_compare_ts(wapts, audioOS->time_base, wvpts, videoOS->time_base) < 0) {
//            writeAudioPacket();
//        } else {
//            writeVideoPacket();
//        }
    }
}


CameraStream::~CameraStream() {
    LOGE(" DESTROY av_write_trailer ");
    av_write_trailer(afc);
}