//
// Created by Administrator on 2018/7/17/017.
//


#include <SeekFile.h>
#include <my_log.h>


SeekFile::SeekFile(AVFormatContext *afc, int audioIndex, int videoIndex) {
    this->afc = afc;
    seekPts = 0;
    this->audioIndex = audioIndex;
    this->videoIndex = videoIndex;
}

void SeekFile::seekStart() {

}

/**
 * @param progress
 * @param audioPts
 * @param videoPts
 */
void SeekFile::seek(float progress , int64_t &audioPts , int64_t &videoPts) {

    seekPts = progress * afc->streams[videoIndex]->duration;
    LOGE("SEEK PRO %f  , PTS %lld , convert_pts %lld  , av_q2d(time_base) %f", progress, seekPts,
         utils.getConvertPts(seekPts, afc->streams[videoIndex]->time_base) , av_q2d( afc->streams[videoIndex]->time_base));

    int result = 0;
    //是视频，还需要自己解码到用户指定位置

    result = av_seek_frame(afc, videoIndex, seekPts, AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        LOGE("av_seek_frame faild %s ", av_err2str(result));
        return;
    }
    int64_t realPts = utils.getConvertPts(seekPts, afc->streams[videoIndex]->time_base);
    //还是直接放在主线程中。子线程不太容易通知其他线程启动。
//    videoPts = findVideoFrame(realPts);
//    audioPts = findAudioFrame(videoPts);
}


int64_t SeekFile::findAudioFrame(int64_t pts) {
    pthread_mutex_lock(&mutex_pthread);
    //把中间的音频数据放进去
    int result = 0;
    int64_t tempPts = 0;
    while (!isExit) {
        if (pause) {
            threadSleep(500);
            continue;
        }
        AVPacket *pkt_ = av_packet_alloc();

        if (pkt_ == NULL) {
            LOGE("READ FRAME av_packet_alloc FAILD !");
            continue;
        }
        result = av_read_frame(afc, pkt_);
        if (result < 0) {
            if (strcmp("End of file", av_err2str(result)) == 0) {
                //文件结尾
//                LOGE(" READ PACKAGE FAILD %s " , av_err2str(result));
            }
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }

        if (pkt_->stream_index == audioIndex) {
            tempPts = utils.getConvertPts(pkt_->pts, afc->streams[audioIndex]->time_base);
            if (pts <= tempPts) {
                LOGE(" AUDIO TIME TEMP PTS   %lld , real pts %lld ", tempPts, pts);
                av_packet_free(&pkt_);
                pkt_ = NULL;
                pthread_mutex_unlock(&mutex_pthread);
                return tempPts;
            }
        } else {
            av_packet_free(&pkt_);
            pkt_ = NULL;
        }
    }
    pthread_mutex_unlock(&mutex_pthread);

}


int64_t SeekFile::findVideoFrame(int64_t pts) {
    pthread_mutex_lock(&mutex_pthread);
    //把中间的音频数据放进去

    int result = 0;
    int64_t tempPts = 0;
    while (!isExit) {

        if (pause) {
            threadSleep(500);
            continue;
        }
        AVPacket *pkt_ = av_packet_alloc();

        if (pkt_ == NULL) {
            LOGE("READ FRAME av_packet_alloc FAILD !");
            continue;
        }

        result = av_read_frame(afc, pkt_);

        if (result < 0) {
            if (strcmp("End of file", av_err2str(result)) == 0) {
                //文件结尾
//                LOGE(" READ PACKAGE FAILD %s " , av_err2str(result));
            }
            threadSleep(2);
            av_packet_free(&pkt_);
            continue;
        }

        if (pkt_->stream_index == videoIndex) {
            tempPts = utils.getConvertPts(pkt_->pts, afc->streams[videoIndex]->time_base);
            if (pts <= tempPts) {
                LOGE(" VIDEO TIME  TEMP PTS %lld , real pts %lld ", tempPts, pts);
                av_packet_free(&pkt_);
                pkt_ = NULL;
                pthread_mutex_unlock(&mutex_pthread);
                return tempPts;
            }
        } else {
            av_packet_free(&pkt_);
            pkt_ = NULL;
        }
    }
    pthread_mutex_unlock(&mutex_pthread);
}


void SeekFile::run() {

}

SeekFile::~SeekFile() {

}

