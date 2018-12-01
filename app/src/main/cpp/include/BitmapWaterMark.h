//
// Created by Administrator on 2018/12/1/001.
//

#ifndef MYFFMPEG_BITMAPWATERMARK_H
#define MYFFMPEG_BITMAPWATERMARK_H

#include "FilterParent.h"

class BitmapWaterMark : public FilterParent{
private :

public :
    BitmapWaterMark(const char *videoPath , const char* logoPath , int x , int y);
    void startWaterMark();
    ~BitmapWaterMark();
};

#endif //MYFFMPEG_BITMAPWATERMARK_H
