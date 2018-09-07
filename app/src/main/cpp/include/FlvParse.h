//
// Created by Administrator on 2018/9/5/005.
//

#ifndef MYFFMPEG_FLVPARSE_H
#define MYFFMPEG_FLVPARSE_H
#include <malloc.h>
#include <stdio.h>
#include <string>
#include "NumUtils.h"
#include "AMF0.h"

using namespace std;

class FlvParse {

private :
    char* path;
    const char* result;
    void getFlvHeader() ;
    void readMetaData(int count);
    void readAudioData(int bodySize);
    void readVideoData(int bodySize);
    int printTagHeader(char *bytes);
    string getVideoCodeId(int type);
    string readMetaDataKey();
    string readMetaDataValue();

    string getSoundFormat(int type);
    string getSoundRate(int type);
    string getSoundSize(int type);
    string getSoundType(int type);

    NumUtils* numUtils;
    AMF0 *amf;
    void readFirstAmf();
    void readSecondAmf();
    void logPreviouTagHeader();

    string resultStr;
    FILE *flv;

public :
    FlvParse(const char* path);
    ~FlvParse();
    const char* start();
};

#endif //MYFFMPEG_FLVPARSE_H
