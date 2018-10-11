//
// Created by Administrator on 2018/9/13/013.
//

#ifndef MYFFMPEG_H264PARSE_H
#define MYFFMPEG_H264PARSE_H

#include <stdio.h>
#include <string>
#include "NumUtils.h"
#include <stdlib.h>
#include <my_log.h>
using namespace std;

/**
 * 先不详细分析sps，pps这些里面的数据。
 * 有点繁杂。
 */
struct SPSstruct{

    char profile_idc;
    //低位
    char constraint_set0_flag : 1;
    char constraint_set1_flag : 1;
    char constraint_set2_flag : 1;
    char constraint_set3_flag : 1;
    char constraint_set4_flag : 1;
    char constraint_set5_flag : 1;
    char reserved_zero_2bit : 2;
    char level_idc;
};

struct NALU{

    int size ;
    int bufSize;
    int startCodeSize;
    char* header;
    char* data;
    bool isEnd;
};

class h264Parse{

private :
    char* path;
    int startCode1(char* buf , int start );
    int startCode2(char* buf, int start  );
    void parseHeader(char *buf , int start);
    NALU* getNalu();


    int naluSize = 2048;
    int nowPosition();
//    void parseSlice();
//    void parseSps();
//    void parsePps();
    void writeMsg(string msg);
    FILE *h264F;
    FILE *h264OutF;
    NumUtils *numUtils ;
public :
    h264Parse(const char *path);
    NALU* getNextNalu();
    ~h264Parse();
    void start();
};

#endif //MYFFMPEG_H264PARSE_H
