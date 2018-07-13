//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_UTILS_H
#define MYFFMPEG_UTILS_H

extern "C"{
#include <libavutil/rational.h>
};


class Utils{

public :
   static int64_t getConvertPts(int64_t pts , AVRational time_base);

};
#endif //MYFFMPEG_UTILS_H
