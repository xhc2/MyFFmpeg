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
    flv = fopen(this->path, "r+");
    if (flv == NULL) {
        resultStr.append("找不到文件！");
        LOGE("cant find file！");
    }
    numUtils = NumUtils::getInstance();
    amf = AMF0::getInstance();
}


const char *FlvParse::start() {
    if (flv != NULL) {
        getFlvHeader();
        int count = 0;
        while(feof(flv) != 1){
            count ++;
            char *tagHeader = (char *)malloc(11);
            if( fread(tagHeader , 1 , 11 , flv) < 11){
                LOGE(" READ HEADER FAILD ! %d " , count );
                break;
            }
            if(tagHeader[0] == 0x12){
                //script tag
                LOGE(" -- meta_data -- ");
                resultStr.append("\n------meta_data------\n");
                printTagHeader(tagHeader);
                readFirstAmf();
                readSecondAmf();
            }
            else if(tagHeader[0] == 0x08){
                //audio
                LOGE(" -- AUDIO -- ");
                resultStr.append("\n\n--------------audio_data--------------");
                int bodySize = printTagHeader(tagHeader);
                readAudioData(bodySize);
            }
            else if(tagHeader[0] == 0x09){
                //video
                LOGE(" -- VIDEO -- ");
                resultStr.append("\n\n--------------video_data--------------");
                int bodySize = printTagHeader(tagHeader);
                readVideoData(bodySize);
            }
            else{
                LOGE(" FAILD TYPE %x " , tagHeader[0]);
                break;
            }
            free(tagHeader);
            logPreviouTagHeader();


        }
    }
    result = resultStr.c_str();
    return result;
}


void FlvParse::readAudioData(int bodySize){
    char *audioData = (char *)malloc(7);
    fread(audioData , 1 , 7 , flv);
    char audioFlag = audioData[0];
    int soundFormat = audioFlag >> 4;
    int soundRate = audioFlag >> 2;
    soundRate &= 3;

    int soundSize = audioFlag >> 1;
    soundSize &= 1;

    int soundType = audioFlag & 1;
    resultStr.append("\n");
    resultStr.append(getSoundFormat(soundFormat));
    resultStr.append(" | ");
    resultStr.append(getSoundRate(soundRate));
    resultStr.append(" | ");
    resultStr.append(getSoundSize(soundSize));
    resultStr.append(" | ");
    resultStr.append(getSoundType(soundType));
    fseek(flv , bodySize - 7  , 1);
    free(audioData);
}

string FlvParse::getSoundFormat(int type){
    string * result;
    switch (type){
        case 0:
            result = new string("Linear PCM, platform endian");
            break;
        case 1:
            result = new string("ADPCM");
            break;
        case 2:
            result = new string("MP3");
            break;
        case 3:
            result = new string("Linear PCM, little endian");
            break;
        case 4:
            result = new string("Nellymoser 16-kHz mono");
            break;
        case 5:
            result = new string(" Nellymoser 8-kHz mono");
            break;
        case 6:
            result = new string("Nellymoser");
            break;
        case 7:
            result = new string(" G.711 A-law logarithmic PCM");
            break;
        case 8:
            result = new string("G.711 mu-law logarithmicPCM");
            break;
        case 9:
            result = new string("reserved");
            break;
        case 10:
            result = new string("AAC");
            break;
        case 11:
            result = new string("Speex");
            break;
        case 14:
            result = new string("MP3 8-Khz");
            break;
        case 15:
            result = new string("Device-specific sound");
            break;
        default:
            result = new string(" unknow ");
            break;
    }
    return *result;
}

string FlvParse::getSoundRate(int type){
    string *result ;
    switch(type){
        case 0:
            result = new string("5.5kz");
            break;
        case 1:
            result = new string("11kz");
            break;
        case 2:
            result = new string("22kz");
            break;
        case 3:
            result = new string("44kz");
            break;
        default:
            result = new string(" unknow ");
            break;
    }
    return *result;
}

string FlvParse::getSoundSize(int type){
    string *str ;
    switch(type){
        case 0:
            str = new string("8bit");
            break;
        case 1:
            str = new string("16bit");
            break;
        default:
            str = new string(" unknow ");
            break;
    }
    return *str;
}

string FlvParse::getSoundType(int type){
    string *str ;
    switch(type){
        case 0:
            str = new string("Mono");
            break;
        case 1:
            str = new string("stereo");
            break;
        default:
            str = new string(" unknow ");
            break;
    }
    return *str;
}

void FlvParse::readVideoData(int bodySize){
    char *videoTag = (char*)malloc(5);
    fread(videoTag , 1 , 5 , flv);
    char type = videoTag[0];
    LOGE(" VIDEO TAG  %x ", type);
    int freameType = type >> 4;
    resultStr.append("\n");
    resultStr.append(numUtils->int2String(bodySize));
    resultStr.append(" | ");
    LOGE("KEY FRAME %d " , freameType);
    switch(freameType)
    {
        case 1:
            resultStr.append(" key frame ");
            break;
        case 2:
            resultStr.append(" inner frame ");
            break;

        case 3:
            resultStr.append(" disposable inner frame(h 263 only) ");
            break;
        case 4:
            resultStr.append(" generated key frame ");
            break;
        default:
            resultStr.append(" unknow ");
            break;
    }
    resultStr.append(" | ");
    int codeId = type & 0x0f;
    resultStr.append(getVideoCodeId(codeId));
    resultStr.append(" | ");
    char pckType = videoTag[1];
    switch(pckType){
        case 0:
            resultStr.append(" sps/pps ");
            break;
        case 1:
            resultStr.append(" nalu ");
            break;
    }
    free(videoTag);
    //第三个参数 ， 0文件头 ， 1当前位置 ， 2文件尾
    fseek(flv , bodySize - 5 , 1);
}

string  FlvParse::getVideoCodeId(int type){
    string *result;
    switch(type){
        case 1:
            result = new string("JPEG");
            break;
        case 2:
            result = new string("Sorenson H.263");
            break;
        case 3:
            result = new string("Screen video");
            break;
        case 4:
            result = new string("On2 VP6");
            break;
        case 5:
            result = new string("On2 VP6 with alpha channel");
            break;
        case 6:
            result = new string("Screen video version 2");
            break;
        case 7:
            result = new string("AVC( H264 )");
            break;
        default:
            result = new string("unknow");
            break;
    }

    return *result;
}

void FlvParse::logPreviouTagHeader(){
    char *tagSize  = (char *)malloc(4);
    fread(tagSize , 1 , 4 , flv);
    int size = numUtils->array2Int(tagSize , 0 , 4);
    LOGE("PREVIOUS TAG SIZE %d " , size );
    free(tagSize);
}

void FlvParse::readFirstAmf(){
    char *firstAmf = (char *)malloc(3);
    resultStr.append(" \n ");
    fread(firstAmf, 1 , 3 , flv);
    int size = amf->readNum(firstAmf , 0 , 3);
    char *amfContent = (char *)malloc(size);
    fread(amfContent, 1 , size , flv);
    string afmBody = amf->getAMF0(firstAmf[0] , amfContent , 0, size);
    LOGE(" FIRST content %s " , afmBody.c_str());
    resultStr.append(afmBody);
    free(firstAmf);
    free(amfContent);
}

void FlvParse::readSecondAmf(){
    char *secondAmf = (char *) malloc(5);
    resultStr.append(" count : ");
    fread(secondAmf, 1 , 5 , flv);
    int size = amf->readNum(secondAmf , 0 , 5);
    LOGE(" MEDATA COUNT %d" ,size);
    resultStr.append(numUtils->int2String(size));
    free(secondAmf);
    readMetaData(size);

}


int FlvParse::printTagHeader(char *tagHeader){
    resultStr.append("\nbody size : ");
    int bodySize = numUtils->array2Int(tagHeader ,1 ,3 );
    resultStr.append(numUtils->int2String(bodySize));

    resultStr.append(" timestamp : ");
    int time = numUtils->array2Int(tagHeader ,4,3 );
    resultStr.append(numUtils->int2String(time));

    resultStr.append(" tExtends : ");
    int extends = numUtils->array2Int(tagHeader ,7,1 );
    resultStr.append(numUtils->int2String(extends));

    resultStr.append(" streamId : ");
    int streamId = numUtils->array2Int(tagHeader ,8 , 3 );
    resultStr.append(numUtils->int2String(streamId));
    return bodySize;
}


void FlvParse::readMetaData(int count) {
    resultStr.append("\n");
    for(int i = 0 ;i < count ; ++ i){
        //开始读取count个属性。
        string key = readMetaDataKey();
        resultStr.append(key);
        resultStr.append(" : ");
        string value = readMetaDataValue();
        resultStr.append(value);
        resultStr.append("\n");
        LOGE(" KEY %s , value %s " , key.c_str() , value.c_str());
    }

    char *endMetaData = (char *)malloc(3);
    fread(endMetaData , 1 , 3 , flv);

    if(endMetaData[0] == 0x00 && endMetaData[1] == 0x00 && endMetaData[2] == 0x09 ){
        LOGE(" META DATA END SUCCESS ");
    }
    else {
        LOGE(" META DATA END FAILD ");
    }
    free(endMetaData);
}

string FlvParse::readMetaDataKey(){
    char *bodySize = (char*)malloc(2);
    fread(bodySize , 1 , 2 , flv);

    int size = numUtils->array2Int(bodySize , 0 , 2);
    char *body = (char *)malloc(size);
    fread(body , 1, size , flv );

    string key = amf->getAMF0(kString , body , 0 , size);
    free(bodySize);
    free(body);
    return key ;
}

string FlvParse::readMetaDataValue(){
    char *type = (char *)malloc(1);
    fread(type , 1 , 1 , flv);
    string result;
    if(*type == kNumber){
        //这里是字节读取八个字节
        char *valueArray = (char *)malloc(8);
        fread(valueArray , 1 ,8 , flv);

        double value = numUtils->array2Double(valueArray , 0);
         result.append(numUtils->double2String(value));
        free(valueArray);
    }
    else if(*type == kBoolean){
        //bool
        char* boolType = (char *)malloc(1);
        fread(boolType , 1 , 1 , flv);
        string str = amf->getAMF0(kBoolean , boolType , 0 , 1);
        result.append(str);
        free(boolType);
    }
    else{
        char *valueSize = (char *)malloc(2);
        fread(valueSize , 1 ,2 , flv);
        int size = numUtils->array2Int(valueSize , 0 , 2);
        char *content = (char *)malloc(size);
        fread(content , 1 , size , flv );
        string str = amf->getAMF0(*type , content , 0 , size );
        result.append(str);
        free(valueSize);
        free(content);
    }

    free(type);

    return result;
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
    free(this->path);
}