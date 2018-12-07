//
// Created by Administrator on 2018/12/7/007.
//

#ifndef MYFFMPEG_VIDEO_H
#define MYFFMPEG_VIDEO_H
#include <vector>
#include <my_log.h>
#include "EditParent.h"

using namespace std;
struct MyInputContext {
    AVFormatContext *afcInput;
    AVCodecContext *vDec;
    AVCodecContext *aDec;
    int videoStreamIndex;
    int audioStreamIndex;
    MyInputContext(){
        afcInput = NULL;
        vDec = NULL;
        aDec = NULL;
    }
    void myInputContextDestroy(){
        if(afcInput != NULL){
            avformat_free_context(afcInput);
        }
        if(vDec != NULL){
            avcodec_free_context(&vDec);
        }
        if(aDec != NULL){
            avcodec_free_context(&aDec);
        }
    }

};

class VideoMerge : public EditParent{

private :
    //输入相关
    vector <char *> inputPaths;
    vector <MyInputContext *> myInputContexts;
    AVFormatContext **afcInputs;

    //输出相关
    AVFormatContext *afcOutput;
    AVCodecContext *vEnconde;
    AVCodecContext *aEnconde;
    int outWidth ;
    int outHeight ;
public :
    VideoMerge(vector<char *> inputPath , const char* output );
    int start();
    int initInput(AVFormatContext **afcInput , char *inputPath);
    int buildOutput(const char *output);
    int buildFilter();
    ~VideoMerge();
};

#endif //MYFFMPEG_VIDEO_H
