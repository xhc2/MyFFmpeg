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

/**
 * 发现播放一次就有1m左右的内存泄漏
 * @param path
 * @param win
 * @param cj
 */
Mp4Player::Mp4Player(const char *path, ANativeWindow *win, CallJava *cj) {
    this->cj = cj;
    video_index = -1;
    audio_index = -1;
    outChannel = 1;

    videoCode = NULL;
    audioCode = NULL;
    readAVPackage = NULL;
    decodeVideo = NULL;
    decodeAudio = NULL;
    audioPlayer = NULL;
    yuvPlayer = NULL;
    seekFile = NULL;
    ac = NULL;
    vc = NULL;
    //必须显式的置null，不然avformat_open_input要报错。
    afc = NULL;
    videoDuration = 0;

    LOGE("------------------------------START-------------------------------");
    int result = initFFmpeg(path);
    if (result < 0) {
        return;
    }
    readAVPackage = new ReadAVPackage(afc, audio_index, video_index);
    decodeVideo = new DecodeVideoThread(afc, vc, video_index);
    decodeAudio = new DeocdeMyAudioThread(ac, afc, audio_index);
    audioPlayer = new NewAudioPlayer(simpleRate, outChannel);
    yuvPlayer = new YuvPlayer(win, outWidth, outHeight);
    seekFile = new SeekFile(afc, audio_index, video_index);

    readAVPackage->setThreadName("ReadAVPackage");
    decodeVideo->setThreadName("decodeVideo");
    decodeAudio->setThreadName("decodeAudio");
    audioPlayer->setThreadName("audioPlayer");
    seekFile->setThreadName("seekFile");

    readAVPackage->addNotify(decodeVideo);
    readAVPackage->addNotify(decodeAudio);
    decodeAudio->addNotify(audioPlayer);
    decodeVideo->addNotify(yuvPlayer);

    readAVPackage->start();
    decodeAudio->start();
    decodeVideo->start();
    audioPlayer->start();
    this->start();

    LOGE("init Mp4Player SUCCESS ");
}

void Mp4Player::seekStart() {
    pauseVA();
}

void Mp4Player::seek(float progress) {
    int result = avformat_flush(afc);
    if (result < 0) {
        LOGE(" avformat_flush result %d ", result);
        return;
    }
    audioPlayer->pts = 0;
    decodeVideo->pts = 0;
    decodeVideo->apts = 0;
    avcodec_flush_buffers(vc);
    avcodec_flush_buffers(ac);
    clearAllQue();
    seekFile->seek(progress, audioPlayer->pts, decodeVideo->pts);
    decodeVideo->apts = audioPlayer->pts;
    playVA();
}

int Mp4Player::initFFmpeg(const char *path) {

    int result = 0;
    av_register_all();
    avformat_network_init();
    LOGE(" play path %s ", path);
    result = avformat_open_input(&afc, path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        cj->callStr("不支持播放！");
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        cj->callStr("不支持播放！");
        return RESULT_FAILD;
    }

    videoDuration = afc->duration / (AV_TIME_BASE / 1000);//毫秒

    LOGE(" video duration %lld ", videoDuration);

//    if(videoDuration <= 0){ rtmp的时候获取不到duration
//        cj->callStr("请检查文件是否被损坏");
//        return RESULT_FAILD;
//    }

    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d ,pix format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            if (avStream->codecpar->format != AV_PIX_FMT_YUV420P) {
                //先暂时不支持 yuv420p以外的格式
                cj->callStr("目前只支持yuv420p的格式");
                return RESULT_FAILD;
            }
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
    LOGE(" AUDIO SIMPKE RATE %d  , channle %d  , format %d , layout %lld ",
         afc->streams[audio_index]->codecpar->sample_rate,
         afc->streams[audio_index]->codecpar->channels,
         afc->streams[audio_index]->codecpar->format,
         afc->streams[audio_index]->codecpar->channel_layout
    );

    if (audioCode == NULL) {
        cj->callStr(" 没找到音频解码器 ");
        return RESULT_FAILD;
    }
    if (videoCode == NULL) {
        cj->callStr(" 没找到视频解码器 ");
        return RESULT_FAILD;
    }
    //回调java层
    char metadata[512];
    sprintf(metadata, "metadata:width=%d,height=%d", afc->streams[video_index]->codecpar->width,
            afc->streams[video_index]->codecpar->height);
    cj->callStr(metadata);

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

//    LOGE(" AUDIO SIMPKE RATE %d  , channle %d  , format %d , layout %lld ", ac->sample_rate,
//         ac->channels, ac->sample_fmt, ac->channel_layout
//    );
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

void Mp4Player::run() {
    while (!isExit) {
        if (pause) {
            threadSleep(1);
            continue;
        }
        //在外面把同步处理了。
        if (audioPlayer != NULL && decodeVideo != NULL) {
            decodeVideo->apts = audioPlayer->pts;
        }
    }
}

void Mp4Player::changeSpeed(float speed) {
    if (audioPlayer != NULL) {
        audioPlayer->changeSpeed(speed);
    }
}

void Mp4Player::clearAllQue() {
    decodeVideo->clearQue();
    decodeAudio->clearQue();
    audioPlayer->clearQue();
}

void Mp4Player::pauseVA() {

    this->setPause();
    if (readAVPackage != NULL) {
        readAVPackage->setPause();
    }
    if (audioPlayer != NULL) {
        audioPlayer->pauseAudio();
        audioPlayer->setPause();
    }
    if (decodeAudio != NULL) {
        decodeAudio->setPause();
    }
    if (decodeVideo != NULL) {
        decodeVideo->setPause();
    }
}

//获取的视频部分。如果音频的长度大于视频的长度，那么这个就不准确了。
int Mp4Player::getProgress() {
    if (decodeVideo == NULL || videoDuration == 0) {
        return 0;
    }
    if (decodeVideo->pts == -100) {
        return -100;
    }
    if (audioPlayer != NULL) {
        if (audioPlayer->pts == -100) {

            return -100;
        }
    }
    return (int) ((float) decodeVideo->pts / (float) videoDuration * 100); //只看了视频部分
}

float Mp4Player::getDuration() {
    return videoDuration / 1000;
}

// 416 235
int Mp4Player::getVideoWidth() {
    if (afc != NULL) {
        return afc->streams[video_index]->codecpar->width;
    }
    return 0;
}

int Mp4Player::getVideoHeight() {
    if (afc != NULL) {
        return afc->streams[video_index]->codecpar->height;
    }
    return 0;
}

void Mp4Player::playVA() {
    LOGE(" PLAY NOW ");
    if (readAVPackage != NULL) {
        readAVPackage->setPlay();
    }
    if (decodeVideo != NULL) {
        decodeVideo->setPlay();
    }
    if (decodeAudio != NULL) {
        decodeAudio->setPlay();
    }
    if (audioPlayer != NULL) {
//        audioPlayer->stop();
        audioPlayer->setPlay();
        audioPlayer->start();
    }
    this->setPlay();
}

Mp4Player::~Mp4Player() {

    LOGE("---------destroy Mp4Player------------");
    videoDuration = -1;
    this->stop();
    if (seekFile != NULL) {
        seekFile->stop();
    }
    if (audioPlayer != NULL) {
        audioPlayer->stop();
    }

    if (decodeAudio != NULL) {
        decodeAudio->stop();
        decodeAudio->removeNotify();
    }
    if (decodeVideo != NULL) {
        decodeVideo->stop();
        decodeVideo->removeNotify();
    }
    if (readAVPackage != NULL) {
        readAVPackage->stop();
        readAVPackage->removeNotify();
    }

    this->join();

    if (audioPlayer != NULL) {
        audioPlayer->join();
    }
    if (decodeAudio != NULL) {
        decodeAudio->join();
    }
    if (decodeVideo != NULL) {
        decodeVideo->join();
    }

    if (readAVPackage != NULL) {
        readAVPackage->join();
    }
    if (seekFile != NULL) {
        seekFile->join();
    }

    if (yuvPlayer != NULL) {
        delete yuvPlayer;
    }
    if (audioPlayer != NULL) {
        delete audioPlayer;
    }
    if (decodeAudio != NULL) {
        delete decodeAudio;
    }
    if (decodeVideo != NULL) {
        delete decodeVideo;
    }
    if (readAVPackage != NULL) {
        delete readAVPackage;
    }
    if (seekFile != NULL) {
        delete seekFile;
    }

    if (vc != NULL) {
        avcodec_close(vc);
    }
    if (ac != NULL) {
        avcodec_close(ac);
    }
    if (afc != NULL) {
        avformat_close_input(&afc);
    }

    LOGE(" DESTROY MP4PLAYER SUCCESS ! ");

}

