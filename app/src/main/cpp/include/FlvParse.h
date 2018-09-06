//
// Created by Administrator on 2018/9/5/005.
//

#ifndef MYFFMPEG_FLVPARSE_H
#define MYFFMPEG_FLVPARSE_H
#include <malloc.h>
#include <stdio.h>
#include <string>
#include "NumUtils.h"

using namespace std;

class FlvParse{

private :
    char* path;
    const char* result;
    void getFlvHeader() ;
    void readMetaData();
    void readAudioData();
    void readVideoData();
    void printTagHeader(char *bytes);

    NumUtils* numUtils;
    void readFirstAmf();
    void readSecondAmf();
    string resultStr;
    FILE *flv;

public :
    FlvParse(const char* path);
    ~FlvParse();
    const char* start();
};

#endif //MYFFMPEG_FLVPARSE_H
