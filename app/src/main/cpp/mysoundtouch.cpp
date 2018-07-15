//
// Created by dugang on 2018/6/29.
//

#include "mysoundtouch.h"
#include "my_log.h"

/**
 * 先用sonic加速普通pcm文件，然后用opengles播放。
 * 然后再用同样的代码来加速MP4文件。
 * @param input_path
 * @return
 */


int mySoundTouch::initFFmpeg(const char *input_path) {
    mutex_pthread =
    int result = 0;
    av_register_all();
    avcodec_register_all();

    LOGE("input path %s ", input_path);
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
            sampleRate = avStream->codecpar->sample_rate;
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


void audioCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    mySoundTouch *ms = (mySoundTouch *) context;
    int size = ms->sonicRead->dealAudio( &ms->getBuf);
    if(size > 0 &&  ms->getBuf != NULL){
        if(size > ms->bufferSize ){
            ms->playAudioBuffer = (short *)realloc(ms->playAudioBuffer , size);
            ms->bufferSize = size;
        }
        memcpy(ms->playAudioBuffer ,ms->getBuf , size );
        fwrite(ms->playAudioBuffer  , 1 ,size ,ms->after );
        (*bf)->Enqueue(bf, ms->playAudioBuffer  , size );
    }

//    if(!ms->audioFrameQue.empty()){
//        MyData myData = ms->audioFrameQue.front();
//        ms->audioFrameQue.pop();
//        memcpy(ms->playAudioBuffer ,myData.data , myData.size );
//        fwrite(ms->playAudioBuffer , 1 , myData.size , ms->after );
//        (*bf)->Enqueue(bf, ms->playAudioBuffer  , myData.size );
//        free(myData.data);
//    }


//    直接读取pcm文件，是正常的
//    int len = 0;
//    int samplesReadBytes = 0;
//    do{
//        len = fread(ms->playAudioBuffer , 1 , 2048 , ms->fReadPcm);
//
//
//        if(len > 0){
//            ms->sonicRead->putSample(ms->playAudioBuffer , len);
//        }
//        else{
//            ms->sonicRead->sonicFlush();
//        }
//        int availiableByte =  ms->sonicRead->availableBytes();
//        LOGE( " availiableByte %d " ,availiableByte );
//        if(availiableByte > 0){
//            if(availiableByte >  ms->bufferSize){
//                //重新分配空间大小
//                ms->playAudioBuffer =  (short *)realloc( ms->playAudioBuffer , ms->bufferSize); bufferSize =
//            }
//            samplesReadBytes = ms->sonicRead->reciveSample(ms->playAudioBuffer ,availiableByte );
//            LOGE( " samplesReadBytes %d " ,samplesReadBytes );
//        }
//        if(samplesReadBytes > 0){
//            fwrite( ms->playAudioBuffer ,1 , samplesReadBytes , ms->after);
//            break;
//        }
//    }while(len > 0);
//
//    (*bf)->Enqueue(bf, ms->playAudioBuffer  , samplesReadBytes );
}




int mySoundTouch::initOpenSl() {
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

    (*pcmQue)->RegisterCallback(pcmQue, audioCallBack, this);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;

}


void mySoundTouch::audioPlayDelay() {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    pthread_mutex_lock(&mutex_pthread);
    threadSleep(300);
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    pthread_mutex_unlock(&mutex_pthread);

}


void mySoundTouch::init(const char *st) {
    int result;
    result = initFFmpeg(st);
    if (RESULT_FAILD == result) {
        LOGE(" initFFmpeg_gpu faild");
        return;
    }
    result = initOpenSl();
    if (RESULT_FAILD == result) {
        LOGE(" initAudio_gpu faild");
        return;
    }

    //1.0是26秒
//    sonicRead = new SonicRead(48000 , 1 , 0.5 , &audioFrameQue);

    after = fopen("sdcard/FFmpeg/after.pcm" , "wb+");
    fReadPcm = fopen("sdcard/FFmpeg/aaaa.pcm" , "r");

    bufferSize = 1024 * 2 * 2;
    playAudioBuffer = (short *) malloc(1024 * 2 * 2);


    readFrameThread = new ReadFrame(&audioPktQue, afc, audioindex);
    readFrameThread->start();

    decodeAudioThread = new DecodeAudioThread(&audioFrameQue, &audioPktQue, ac, afc, audioindex , swc, aframe);
    decodeAudioThread->start();

    this->start();

}

void mySoundTouch::run() {
    audioPlayDelay();
}

mySoundTouch::~mySoundTouch(){
    readFrameThread->stop();
    decodeAudioThread->stop();
    this->stop();
}