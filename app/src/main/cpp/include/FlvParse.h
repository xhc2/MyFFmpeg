//
// Created by Administrator on 2018/9/5/005.
//

#ifndef MYFFMPEG_FLVPARSE_H
#define MYFFMPEG_FLVPARSE_H
#include <malloc.h>
#include <stdio.h>
#include <string>

using namespace std;

class FlvParse{

private :
    const char* path;
    const char* result;
    void getFlvHeader() ;
    void readMetaData();
    void readAudioData();
    void readVideoData();
    string int2String(int num);
    double toDouble(char *bytes , int size);
    int array2Int(char *array , int start , int size);
    int array2Double(char *array);
    string resultStr;
    FILE *flv;
public :
    FlvParse(const char* path);
    ~FlvParse();
    const char* start();
};

#endif //MYFFMPEG_FLVPARSE_H
