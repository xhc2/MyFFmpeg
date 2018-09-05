//
// Created by Administrator on 2018/9/5/005.
//


#include <my_log.h>
#include <stdlib.h>
#include <sstream>
#include "FlvParse.h"


FlvParse::FlvParse(const char *path) {
    this->path = path;
    flv = fopen(path, "r");
    if (flv == NULL) {
        resultStr.append("找不到文件！");
        LOGE("cant find file！");
    }
}


double FlvParse::toDouble(char *bytes , int size){
    if(size <= 0) return -1;
    char temp[size] ;
    for(int i = 0 ; i < size ; ++ i){
        temp[i] = bytes[i];
    }
    return  *(double*)temp;

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
                resultStr.append("---header---");
                resultStr.append(" body size : ");
                int bodySize = array2Int(tagHeader ,1 ,3 );
                resultStr.append(int2String(bodySize));
                resultStr.append(" timestamp : ");

                int time = array2Int(tagHeader ,5,3 );
                resultStr.append(int2String(time));

                int streamId = array2Int(tagHeader ,9 , 3 );
                resultStr.append(int2String(streamId));

            }
            else if(tagHeader[0] == 0x08){
                //audio
                resultStr.append("---audio_data---");

            }
            else if(tagHeader[0] == 0x09){
                //video
                resultStr.append("---video_data---");


            }
            break;//test
        }
    }

    result = resultStr.c_str();
    return result;
}


int FlvParse::array2Int(char *array , int start , int size){
    int result = 0;
    char temp[size] ;
    for(int i = start ;i < start+size ; ++ i){
        temp[i - start] = array[i];
    }
    memcpy(&result , temp , size);
    return result;
}

string FlvParse::int2String(int num){
    stringstream stream;
    stream<<num;
    return stream.str();
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
    resultStr.append(int2String(version));
    free(temp);
}




FlvParse::~FlvParse() {

}