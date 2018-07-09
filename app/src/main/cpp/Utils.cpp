//
// Created by dugang on 2018/6/29.
//
#include "Utils.h"

int Utils::getConvertPts(int64_t pts, AVRational time_base) {
    return (int) (pts * av_q2d(time_base) * 1000);
}
