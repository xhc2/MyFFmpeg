//
// Created by Administrator on 2018/7/17/017.
//


#include <SeekFile.h>
#include <my_log.h>


SeekFile::SeekFile(AVFormatContext *afc) {
    this->afc = afc;
    seekPts = 0;

}

void SeekFile::seekStart(){

}

void SeekFile::seek(float progress , int streamIndex , bool isAudio){
    seekPts = progress *  afc->streams[streamIndex]->duration;

    LOGE("SEEK PRO %f  , PTS %lld , convert_pts %lld " , progress , seekPts ,
         utils.getConvertPts(seekPts, afc->streams[streamIndex]->time_base));
    int result = 0;
    if(isAudio){
        //是音频类的

    }
    else{
        //是视频，还需要自己解码到用户指定位置

        result = av_seek_frame(afc ,streamIndex , seekPts ,  AVSEEK_FLAG_BACKWARD  );
        if(result < 0){
            LOGE("av_seek_frame faild %s " , av_err2str(result));
            return ;
        }
        int64_t realPts =  utils.getConvertPts(seekPts, afc->streams[streamIndex]->time_base);
        //还是直接放在主线程中。子线程不太容易通知其他线程启动。
        findFrame(realPts , streamIndex);
    }
}

void SeekFile::findFrame(int64_t pts , int streamIndex){
    pthread_mutex_lock(&mutex_pthread);
    int result = 0 ;
    int64_t tempPts = 0;
    while (!isExit) {

        if (pause) {
            threadSleep (500);
            continue;
        }
        AVPacket *pkt_ = av_packet_alloc();
        if(pkt_ == NULL){
            LOGE("READ FRAME av_packet_alloc FAILD !");
            continue;
        }
        result = av_read_frame(afc, pkt_);

        if (result < 0) {
            if(strcmp("End of file"  , av_err2str(result)) == 0){
                //文件结尾
//                LOGE(" READ PACKAGE FAILD %s " , av_err2str(result));
            }
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }
        tempPts = utils.getConvertPts(pkt_->pts ,afc->streams[streamIndex]->time_base);

        if (pkt_->stream_index == streamIndex && pts <= tempPts) {
            LOGE(" TEMP PTS %lld , real pts %lld " , tempPts , pts);
            LOGE(" FIND REAL FRAME !");
            av_packet_free(&pkt_);
            pkt_= NULL;
            pthread_mutex_unlock(&mutex_pthread);
           return ;
        } else {
            av_packet_free(&pkt_);
            pkt_= NULL;
        }

    }
    pthread_mutex_unlock(&mutex_pthread);
}


void SeekFile::run() {

}

SeekFile::~SeekFile() {

}

