//
// Created by Administrator on 2018/9/13/013.
//

#include "h264Parse.h"
#include <stdlib.h>
#include <my_log.h>

h264Parse::h264Parse(const char* path){
    int length = strlen(path);
    this->path = (char *)malloc(length);
    memcpy(this->path , path , length);
    LOGE("PATH %s " , path);
    h264F = fopen(path , "r");
    if(h264F == NULL){
        LOGE(" FILE IS NULL !");
        return ;
    }
    h264OutF = fopen("sdcard/FFmpeg/h264parse.txt" , "wb+");
    if(h264OutF == NULL){
        LOGE(" h264 file faild !");
        return ;
    }
    numUtils = NumUtils::getInstance();

    char *temp = (char *)malloc(7);
    temp[6] = 0;
    temp[5] = 0;
    temp[4] = 0;
    temp[3] = 1;
    temp[2] = 0;
    temp[1] = 1;
    temp[0] = 1;

    int result =numUtils->columbusCoding(temp);
    LOGE(" result %d " , result);
}

bool h264Parse::isHead(){
    char *head = (char *)malloc(3);
    fread(head , 1 , 3 , h264F);
    if(head[0] == 0x00 && head[1] == 0x00 && head[2] == 0x01){
        free(head);
        return true;
    }
    if( fseek(h264F , -3 ,1) == -1){
        LOGE(" seek faild !");
        free(head);
        return false;
    }

    head = (char *)realloc(head , 4);

    if(head[0] == 0x00 && head[1] == 0x00 && head[2] == 0x00 && head[3] == 0x01){
        free(head);
        return true;
    }
    free(head);
    return false;
}

void h264Parse::parseHeader(){
    writeMsg("\n");
    char *head = (char *)malloc(1);
    int nalHead = *head ;
    int forbidden = nalHead & 0x80;
    if(forbidden != 0){
        LOGE(" BORBIDDEN IS WRONG ");
        return ;
    }
    int nri = nalHead & 0x60;
    string str(" nal_ref_idc：");
    str.append(numUtils->int2String(nri));
    writeMsg(str);
    int naluType = nalHead * 0x1F;
    str.clear();
    str.append("type : ");
    str.append(numUtils->int2String(naluType));
    str.append("（");
    switch(naluType){
        case 1:
            str.append(" SLICE ");
            break;
        case 2:
            str.append("SLICE A");
            break;
        case 3:
            str.append("SLICE B");
            break;
        case 4:
            str.append("SLICE C");

            break;
        case 5:
            str.append(" IDR ");

            break;
        case 6:
            str.append(" SEI ");

            break;
        case 7:
            str.append(" SPS ");

            break;
        case 8:
            str.append(" PPS ");

            break;
        case 9:
            str.append(" 分界符 ");

            break;
        case 10:
            str.append(" 序列结束 ");

            break;
        case 11:
            str.append(" 码流结束 ");
            break;
        case 12:
            str.append(" 填充 ");
            break;
    }
    str.append("）");
    writeMsg(str);
    free(head);
}

void h264Parse::parseSliceHeader(){

}

void h264Parse::writeMsg(string msg){
    fwrite(msg.c_str() , 1, msg.length() , h264OutF);
    fflush(h264OutF);
}

void h264Parse::start() {


}