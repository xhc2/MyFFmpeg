//
// Created by dugang on 2018/7/3.
//

/**
 * 视频播放的主类
 * 1.opengles播放yuv画面（openGlES 一个类）
 * 2.opensles播放音频   （opensl 一个类）
 * 3.读取avpacket        （读取一个类）
 * 4.解码音频
 * 5.解码视频
 * 6.倍速播放
 */

#include <Mp4Player.h>
#include <my_log.h>
#include <ReadAvPacket.h>
#include <DecodeVideoThread.h>
#include <android/native_window.h>
#include <YuvPlayer.h>


Mp4Player::Mp4Player(const char* playPath ,ANativeWindow* win){
     video_index = -1;
     audio_index = -1;
    this->playPath = playPath;
    initFFmpeg();
//    readAVPackage = new ReadAVPackage(afc , audio_index , video_index);
//    decodeVideo = new DecodeVideoThread(afc ,vc , video_index);
//    decodeAudio = new DeocdeMyAudioThread(ac , afc , audio_index);
//    audioPlayer = new AudioPlayer(simpleRate , outChannel);
//    LOGE(" OUT WIDTH %d , OUTHEIGHT %d " , outWidth , outHeight);
//    yuvPlayer = new YuvPlayer(win , outWidth , outHeight);


//    readAVPackage->addNotify(decodeVideo);
//    readAVPackage->addNotify(decodeAudio);
//    decodeAudio->addNotify(audioPlayer);
//    decodeVideo->addNotify(yuvPlayer);
//
//    readAVPackage->start();
//    decodeAudio->start();
//    decodeVideo->start();
//    audioPlayer->start();
//    this->start();
}


int Mp4Player::initFFmpeg() {
    int result = 0;
    av_register_all();
    avcodec_register_all();
    LOGE(" input path %s ", playPath);
    result = avformat_open_input(&afc, playPath, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    videoDuration = afc->duration / (AV_TIME_BASE / 1000);

    LOGE(" video duration %lld ", videoDuration);

    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d , format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!videoCode) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audio_index = i;

            this->simpleRate = avStream->codecpar->sample_rate;
            LOGE("audio samplerate %d ", avStream->codecpar->sample_rate);
            audioCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            if (!audioCode) {
                LOGE("audio avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }

    ac = avcodec_alloc_context3(audioCode);
    if (!ac) {
        LOGE("ac AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    vc = avcodec_alloc_context3(videoCode);
    if (!vc) {
        LOGE("vc AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc, afc->streams[video_index]->codecpar);
    avcodec_parameters_to_context(ac, afc->streams[audio_index]->codecpar);

    vc->thread_count = 4;
    ac->thread_count = 4;

    result = avcodec_open2(vc, NULL, NULL);
    if (result != 0) {
        LOGE("vc avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    result = avcodec_open2(ac, NULL, NULL);
    if (result != 0) {
        LOGE("ac avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    outWidth = vc->width;
    outHeight = vc->height;

    LOGE("outwidth %d , outheight %d ", outWidth, outHeight);




    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}

void Mp4Player::run(){
    while(!isExit){
        if(pause){
            threadSleep(2);
            continue;
        }
        //在外面把同步处理了。
        if(audioPlayer != NULL && decodeVideo != NULL){
            decodeVideo->apts = audioPlayer->pts;
        }
    }
}

Mp4Player::~Mp4Player(){

}

