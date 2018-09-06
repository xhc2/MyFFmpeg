//
// Created by Administrator on 2018/9/5/005.
//


#include <my_log.h>
#include <stdlib.h>

#include "FlvParse.h"


FlvParse::FlvParse(const char *p) {
    int len = strlen(p);
    this->path = (char *)malloc(len);

    memcpy(this->path , p , len);

    LOGE(" path %s " , this->path);
    flv = fopen(this->path, "r+");
    if (flv == NULL) {
        resultStr.append("找不到文件！");
        LOGE("cant find file！");
    }
    numUtils = NumUtils::getInstance();
}


const char *FlvParse::start() {
    if (flv != NULL) {
        getFlvHeader();
        while(true){
            char *tagHeader = (char *)malloc(11);
            fread(tagHeader , 1 , 11 , flv);
            if(tagHeader[0] == 0x12){
                //script tag
                resultStr.append("\n---meta_data---\n");
                printTagHeader(tagHeader);
                readFirstAmf();
                readSecondAmf();
            }
            else if(tagHeader[0] == 0x08){
                //audio
                resultStr.append("---audio_data---");
                printTagHeader(tagHeader);


            }
            else if(tagHeader[0] == 0x09){
                //video
                resultStr.append("---video_data---");
                printTagHeader(tagHeader);
            }
            free(tagHeader);
            break;//test
        }
    }

    result = resultStr.c_str();
    return result;
}

void FlvParse::readFirstAmf(){
    char *firstAmf = (char *)malloc(3);

    fread(firstAmf, 1 , 3 , flv);


    free(firstAmf);
}

void FlvParse::readSecondAmf(){

}


void FlvParse::printTagHeader(char *tagHeader){
    resultStr.append("---header---");
    resultStr.append(" body size : ");
    int bodySize = numUtils->array2Int(tagHeader ,1 ,3 );
    resultStr.append(numUtils->int2String(bodySize));

    resultStr.append(" timestamp : ");
    int time = numUtils->array2Int(tagHeader ,5,3 );
    resultStr.append(numUtils->int2String(time));

    resultStr.append(" streamId : ");
    int streamId = numUtils->array2Int(tagHeader ,9 , 3 );
    resultStr.append(numUtils->int2String(streamId));
}


void FlvParse::readMetaData() {

}

void FlvParse::readAudioData() {

}

void FlvParse::readVideoData() {

}

void FlvParse::getFlvHeader() {
    resultStr.append("-------flvheader--------\n");
    char *temp = (char *) malloc(13);
    fread(temp, 1, 13, flv);
    if (temp[0] != 'F' || temp[1] != 'L' || temp[2] != 'V') {
        resultStr.clear();
        resultStr.append("-------不是flv文件--------");
        return;
    }
    int version = temp[3];
    resultStr.append("version : " );
    resultStr.append(numUtils->int2String(version));
    free(temp);
}




FlvParse::~FlvParse() {

}