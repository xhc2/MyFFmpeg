//
// Created by dugang on 2018/6/29.
//
#include <my_log.h>
#include "Utils.h"

int64_t Utils::getConvertPts(int64_t pts, AVRational time_base) {
    int64_t result = (int64_t)(av_q2d(time_base) * 1000 * pts );
//    LOGE(" PTS %lld   " , pts  );
    return  result;
}
