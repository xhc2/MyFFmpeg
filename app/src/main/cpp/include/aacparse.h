//
// Created by Administrator on 2018/10/10/010.
//

#ifndef MYFFMPEG_AACPARSE_H
#define MYFFMPEG_AACPARSE_H
#include <string.h>
#include <stdio.h>
#include <my_log.h>
struct AACFrame{
    int size ;
    char* data;
};

struct FrameHead{
    unsigned short syncword ; //12
    unsigned char mpegversion : 1;
    unsigned char layer : 2;
    unsigned char protectionabsent : 1;
    unsigned char profile : 2;
    unsigned char mpeg4fi : 4;
    unsigned char privateStream : 1;
    unsigned char mpeg4cc : 3;
    unsigned char originality : 1;
    unsigned char home : 1;
    unsigned char copyrightedStream : 1;
    unsigned char copyrightStart : 1;
    unsigned short frameLength : 13;
    unsigned short bufferFullness : 11;
    unsigned char numOfAACFrame : 2;
    //crc , 如果protectionabsent == 0 还有两个字节的crc

};

class AACParse{

private :
    FILE *aacF;
public :
    int parseAACHeader(bool write);
    AACFrame* getAACFrame(bool write);
    void parseStart();
    AACParse(const char* path);
    ~AACParse();
};


#endif //MYFFMPEG_AACPARSE_H
