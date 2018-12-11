//
// Created by Administrator on 2018/8/18/018.
//


#include "EditParent.h"

class DecodeTest : public EditParent{

private :
    AVFormatContext *afcOutput;
    AVCodecContext *vCtxE;
public :
    DecodeTest();
    int  buildOutput(const char *outputPath) ;
};