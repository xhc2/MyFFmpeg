//
// Created by Administrator on 2018/12/5/005.
//

#ifndef MYFFMPEG_GIFMAKE_H
#define MYFFMPEG_GIFMAKE_H

#include "EditParent.h"
extern "C"{
#include <libavcodec/gif.h>
};
class GifMake : public EditParent{
private:
    AVFormatContext *afc_output ;
public:
    GifMake(const char *inputPath , const char* outPath);
    ~GifMake();
};

#endif //MYFFMPEG_GIFMAKE_H
