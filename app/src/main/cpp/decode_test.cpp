#include <my_log.h>
#include "decode_test.h"
extern "C"{
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
}

DecodeTest::DecodeTest(){
//    const char *inputName = "sdcard/FFmpeg/aaaaa.mp4";
    const char *inputName ="sdcard/FFmpeg/cameraStream.flv";
    SwrContext *swc = NULL;
    AVFormatContext *afc = NULL;
    AVCodec *audioCode = NULL;
    AVCodecContext *ac = NULL;
    AVFrame *avFrame = av_frame_alloc();
    FILE *file = fopen("sdcard/FFmpeg/pcm.pcm" , "wb+");
    uint8_t *play_audio_temp = (uint8_t *)malloc(1024 * 2 * 1);
    int audioIndex = -1;
    int result ;
    av_register_all();
    result = avformat_open_input(&afc ,inputName  , NULL , NULL );
    if(result < 0){
        LOGE(" avformat_open_input %s " , av_err2str(result));
        return ;
    }

    result = avformat_find_stream_info(afc, 0);

    if (result != 0) {
        LOGE("avformat_find_stream_info failed!:%s", av_err2str(result));
        return ;
    }

    for(int i = 0 ; i < afc->nb_streams ; ++ i){
        AVStream *avStream = NULL;
        avStream =  afc->streams[i];
        if(afc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            LOGE(" FIND AUDIO TYPE ");

            audioCode = avcodec_find_decoder(avStream->codecpar->codec_id);
            LOGE(" AUDIO CODE NAME %d " , audioCode->sample_fmts);
            audioIndex = i;
        }
    }
    if(audioCode == NULL){
        LOGE(" 没找到音频解码器 ");
        return ;
    }
    ac = avcodec_alloc_context3(audioCode);
    if (!ac) {
        LOGE("ac AVCodecContext FAILD ! ");
        return ;
    }
    avcodec_parameters_to_context(ac, afc->streams[audioIndex]->codecpar);
    ac->channels = 1;
    ac->thread_count = 4;

    result = avcodec_open2(ac, NULL, NULL);
    if (result != 0) {
        LOGE("ac avcodec_open2 Faild !");
        return ;
    }
    swc = swr_alloc_set_opts(NULL,
                             av_get_default_channel_layout(1),
                             AV_SAMPLE_FMT_S16, ac->sample_rate,
                             av_get_default_channel_layout(ac->channels),
                             ac->sample_fmt, ac->sample_rate,
                             0, 0);
    if (swr_init(swc) < 0) {
        LOGE(" swr_init FAILD !");
    }
    LOGE(" AC SMAPLERATE %d , " , ac->sample_rate);
    LOGE(" AC bit_rate %d , " , ac->bit_rate);
    LOGE(" AC channels %d , " , ac->channels);
    LOGE(" AC channel_layout %d , " , ac->channel_layout);
    LOGE(" AC  ac->sample_fmt %d , " , ac->sample_fmt);
    while(true){
        AVPacket *pkt = av_packet_alloc();
        result = av_read_frame(afc, pkt);
        if(result < 0){
            LOGE(" READ FRAME FAILD %s " , av_err2str(result));
            break;
        }

        result = avcodec_send_packet(ac, pkt);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            av_packet_free(&pkt);
            continue;
        }
        av_packet_free(&pkt);
        while (true) {
            result = avcodec_receive_frame(ac, avFrame);
            if (result < 0) {
                break;
            }
            LOGE("DECODE FRAME ");
            uint8_t *out[1] = {0};
            out[0] = (uint8_t *) play_audio_temp;
//            MyData *myData = new MyData();
            //音频重采样
            swr_convert(swc, out,
                        avFrame->nb_samples,
                        (const uint8_t **) avFrame->data,
                        avFrame->nb_samples);

            fwrite(play_audio_temp , 1 , avFrame->nb_samples * 2 , file);
        }
    }

}




















