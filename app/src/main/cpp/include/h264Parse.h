//
// Created by Administrator on 2018/9/13/013.
//

#ifndef MYFFMPEG_H264PARSE_H
#define MYFFMPEG_H264PARSE_H

#include <stdio.h>
#include <string>
#include "NumUtils.h"

using namespace std;

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

class h264Parse{

private :
    char* path;
    bool isHead();
    void parseHeader();
    void parseSliceHeader();
    void writeMsg(string msg);

    FILE *h264F;
    FILE *h264OutF;
    NumUtils *numUtils ;
public :
    h264Parse(const char *path);
    void start();
};

#endif //MYFFMPEG_H264PARSE_H
