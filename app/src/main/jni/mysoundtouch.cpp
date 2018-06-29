//
// Created by dugang on 2018/6/29.
//

#include "mysoundtouch.h"
#include "my_log.h"


int mySoundTouch::init_sound_touch(int sampleRate) {

//    int size = 48000;
//    reciveBuf = (SAMPLETYPE *) malloc(size * 2);
//    putbuffer = (SAMPLETYPE *) malloc(size * 2);
//    buf_play = (SAMPLETYPE *) malloc(size * 2);
//    play_audio_temp = (char*)malloc( 2 * size);

    soundTouch = new SoundTouch();
    //采样率
    soundTouch->setSampleRate(sampleRate);
    //声道数
    soundTouch->setChannels(1);
    //速度
    soundTouch->setTempo(1.0);
    //声调
    soundTouch->setPitch(1);

    return RESULT_SUCCESS;
}

int mySoundTouch::initFFmpeg(const char *input_path){

    int result = 0;
    av_register_all();
    avcodec_register_all();

    LOGE("input path %s " , input_path);
    aframe = av_frame_alloc();
    result = avformat_open_input(&afc, input_path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        return RESULT_FAILD;
    }

    duration = afc->duration / (AV_TIME_BASE / 1000);

    LOGE(" video duration %lld ", duration);

    for (int i = 0; i < afc->nb_streams; ++i) {
        AVStream *avStream = afc->streams[i];
         if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audioindex = i;
            LOGE("audio samplerate %d ", avStream->codecpar->sample_rate);
            audioCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            init_sound_touch(avStream->codecpar->sample_rate);
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


    //将codec中的参数放进accodeccontext
//    avcodec_parameters_to_context(vc, afc->streams[video_index]->codecpar);
    avcodec_parameters_to_context(ac, afc->streams[audioindex]->codecpar);

    ac->thread_count = 4;


    result = avcodec_open2(ac, NULL, NULL);
    if (result != 0) {
        LOGE("ac avcodec_open2 Faild !");
        return RESULT_FAILD;
    }


    //音频重采样上下文初始化 , AV_SAMPLE_FMT_S16 格式的单声道
    swc = swr_alloc_set_opts(NULL,
                                 av_get_default_channel_layout(1),
                                 AV_SAMPLE_FMT_S16, ac->sample_rate,
                                 av_get_default_channel_layout(ac->channels),
                             ac->sample_fmt, ac->sample_rate,
                                 0, 0);
    result = swr_init(swc);

    if (result < 0) {
        LOGE(" swr_init FAILD !");
        return RESULT_FAILD;
    }
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}


// audio part
SLEngineItf mySoundTouch::createOpenSL() {
    SLresult re = 0;
    SLEngineItf en = NULL;

    re = slCreateEngine(&engineOpenSL, 0, 0, 0, 0, 0);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return NULL;
    }

    re = (*engineOpenSL)->Realize(engineOpenSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return NULL;
    }

    re = (*engineOpenSL)->GetInterface(engineOpenSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return NULL;
    }
    return en;
}

int mySoundTouch::initOpenSl(){
    //创建引擎
    eng = createOpenSL();
    if (!eng) {
        LOGE("createSL FAILD ");
    }
    //2.创建混音器
    mix = NULL;
    SLresult re = 0;
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};


    //音频格式
    SLDataFormat_PCM pcm_ = {
            SL_DATAFORMAT_PCM,
            1,//    声道数
            SL_SAMPLINGRATE_48,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm_};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                       sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return RESULT_FAILD;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue)->RegisterCallback(pcmQue, pcmCallBack, 0);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;

}





void mySoundTouch::pcmCallBack(SLAndroidSimpleBufferQueueItf bf, void *context){

}

void mySoundTouch::init(const char *st) {
    int result ;
    result = initFFmpeg(st);
    if (RESULT_FAILD == result) {
        LOGE(" initFFmpeg_gpu faild");
        return ;
    }
    result = initOpenSl();
    if (RESULT_FAILD == result) {
        LOGE(" initAudio_gpu faild");
        return  ;
    }

    readFrameThread = new ReadFrame(&audioPktQue , afc , audioindex);
    readFrameThread->start();

    decodeAudioThread = new DecodeAudioThread();
    decodeAudioThread->start();


}

void mySoundTouch::run() {
    LOGE(" RUN ING ");


}