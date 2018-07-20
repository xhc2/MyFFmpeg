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



Mp4Player::Mp4Player(const char* path , ANativeWindow* win){
     video_index = -1;
    audio_index = -1;
    outChannel = 1 ;
    //必须显式的置null，不然avformat_open_input要报错。
    afc = NULL;
    LOGE("------------------------------START-------------------------------");
    int result = initFFmpeg(path);
//    if(result < 0){
//        return ;
//    }
    readAVPackage = new ReadAVPackage(afc , audio_index , video_index);
    decodeVideo = new DecodeVideoThread(afc ,vc , video_index);
    decodeAudio = new DeocdeMyAudioThread(ac , afc , audio_index);
    audioPlayer = new AudioPlayer(simpleRate , outChannel);
    yuvPlayer = new YuvPlayer(win , outWidth , outHeight);
    seekFile = new SeekFile(afc  , audio_index , video_index );

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

void Mp4Player::seekStart(){
    pauseVA();
}

void Mp4Player::seek(float progress){
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
    seekFile->seek(progress , audioPlayer->pts ,  decodeVideo->pts);
    decodeVideo->apts = audioPlayer->pts;
    playVA();
}

int Mp4Player::initFFmpeg(const char* path) {

    int result = 0;
    av_register_all();
    avcodec_register_all();
    result = avformat_open_input(&afc, path , 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    videoDuration = afc->duration / (AV_TIME_BASE / 1000);//毫秒

    LOGE(" video duration %lld ", videoDuration);

    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d ,pix format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode = avcodec_find_decoder(avStream->codecpar->codec_id);

            if(avStream->codecpar->format != AV_PIX_FMT_YUV420P){
                //先暂时不支持 yuv420p以外的格式
                return RESULT_WRONG_PIX;
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
            threadSleep(1);
            continue;
        }
        //在外面把同步处理了。
        if(audioPlayer != NULL && decodeVideo != NULL){
            decodeVideo->apts = audioPlayer->pts;
        }
    }
}

void Mp4Player::changeSpeed(float speed){
    if(audioPlayer != NULL){
        audioPlayer->changeSpeed(speed);
    }
}

void Mp4Player::clearAllQue(){
    decodeVideo->clearQue();
    decodeAudio->clearQue();
    audioPlayer->clearQue();
}

void Mp4Player::pauseVA(){

    this->setPause();
    if(readAVPackage != NULL){
        readAVPackage->setPause();
    }
    if(audioPlayer != NULL){
        audioPlayer->pauseAudio();
         audioPlayer->setPause();
    }
    if(decodeAudio != NULL){
        decodeAudio->setPause();
    }
    if(decodeVideo != NULL){
        decodeVideo->setPause();
    }
}

int Mp4Player::getProgress(){
    return (int)((float)audioPlayer->pts / (float)videoDuration * 100);
}

float Mp4Player::getDuration(){
    return videoDuration / 1000;
}

void Mp4Player::playVA(){
    LOGE(" PLAY NOW ");
    if(readAVPackage != NULL){
        readAVPackage->setPlay();
    }
    if(decodeVideo != NULL){
        decodeVideo->setPlay();
    }
    if(decodeAudio != NULL){
        decodeAudio->setPlay();
    }
    if(audioPlayer != NULL){
//        audioPlayer->stop();
        audioPlayer->setPlay();
        audioPlayer->start();
    }
    this->setPlay();
}

Mp4Player::~Mp4Player(){
    videoDuration = -1;
    seekFile->stop();
    this->stop();
    if(audioPlayer != NULL){
        audioPlayer->stop();
    }

    if(decodeAudio != NULL){
        decodeAudio->stop();
    }
    if(decodeVideo != NULL){
        decodeVideo->stop();
    }
    if(readAVPackage != NULL){
        readAVPackage->stop();
    }

    readAVPackage->removeNotify();
    decodeAudio->removeNotify();
    decodeVideo->removeNotify();


    this->join();
    audioPlayer->join();
    decodeAudio->join();
    decodeVideo->join();
    readAVPackage->join();
    seekFile->join();

    delete yuvPlayer;
    delete audioPlayer;
    delete decodeAudio;
    delete decodeVideo;
    delete readAVPackage;
    delete seekFile;

    if (vc != NULL) {
        avcodec_close(vc);
    }
    if (ac != NULL) {
        avcodec_close(ac);
    }
    if (afc != NULL) {
        avformat_close_input(&afc);
    }

    LOGE("destroy Mp4Player");
}

