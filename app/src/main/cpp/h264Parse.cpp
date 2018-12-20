// Created by Administrator on 2018/9/13/013.

#include "h264Parse.h"


h264Parse::h264Parse(const char *path) {
    h264F = fopen(path, "r");
    if (h264F == NULL) {
        LOGE(" FILE IS NULL !");
        return;
    }
    h264OutF = fopen("sdcard/FFmpeg/fileparse/h264parse.txt", "wb+");
    if (h264OutF == NULL) {
        LOGE(" h264 file faild !");
        return;
    }
    numUtils = NumUtils::getInstance();

}


//3位的code
int h264Parse::startCode1(char *buf, int start) {
    if (buf[start] == 0x00 && buf[start + 1] == 0x00 && buf[start + 2] == 0x01) {
        return 3;
    }
    return -1;
}

//4位的code
int h264Parse::startCode2(char *buf, int start) {
    if (buf[start] == 0x00 && buf[start + 1] == 0x00 && buf[start + 2] == 0x00 &&
        buf[start + 3] == 0x01) {
        return 4;
    }
    return -1;
}

NALU *h264Parse::getNalu() {
    if(h264F == NULL){
        return NULL ;
    }
    NALU *nalu = new NALU();
    nalu->size = 0;
    nalu->bufSize = 0;
    nalu->startCodeSize = 0;
    nalu->isEnd = false;
    char *tempBuf = (char *) malloc(naluSize);
    nalu->bufSize = naluSize;
    int len = fread(tempBuf, 1, 3, h264F);
    if (len == -1 || len < 3) {
        return NULL;
    };

    int startCode = startCode1(tempBuf, 0);
    if (startCode == -1) {
        fread(tempBuf + 3, 1, 1, h264F);
        startCode = startCode2(tempBuf, 0);
        if (startCode == -1) {
            free(tempBuf);
            return NULL;
        }
    }
    nalu->startCodeSize = startCode;
    nalu->size += startCode;

    startCode = -1;
    while (startCode == -1) {
        if (feof(h264F)) {
            //文件结尾也算是个startcode标识
            LOGE(" END OF FILE ! ");
            nalu->isEnd = true;
            fclose(h264F);
            break;
        }

        if (nalu->size + 1 > nalu->bufSize) {
            //万一空间不够，重新开辟
            char *temp = (char *) malloc(nalu->size + naluSize);
            nalu->bufSize = nalu->size + naluSize;
            memcpy(temp, tempBuf, nalu->size);
            free(tempBuf);
            tempBuf = temp;
        }

        fread(tempBuf + nalu->size, 1, 1, h264F);
        nalu->size++;
        startCode = startCode2(tempBuf, nalu->size - 4);
        if (startCode == -1) {
            startCode = startCode1(tempBuf, nalu->size - 3);
            if (startCode != -1) {
                break;
            }
        } else break;
    }
    nalu->size -= startCode;
    if (fseek(h264F, -startCode, 1) == -1) {
        LOGE(" SEEK FAILD !");
    }
    nalu->size -= nalu->startCodeSize;
    nalu->data = tempBuf;

    return nalu;
}

//判断nalu类型
void h264Parse::parseHeader(char *buf, int start) {
    writeMsg("\n");
    char nalHead = buf[start];
    LOGE(" nal head %x ", nalHead);
    char forbidden = nalHead & 0x80;
    if (forbidden != 0) {
        LOGE(" BORBIDDEN IS WRONG ");
        return;
    }
    int nri = nalHead & 0x60;
    nri = nri >> 5;
    string str(" nal_ref_idc: ");
    str.append(numUtils->int2String(nri));
    writeMsg(str);
    int naluType = nalHead & 0x1F;
    str.clear();
    str.append("| type : ");
    str.append(numUtils->int2String(naluType));
    str.append("( ");
    switch (naluType) {
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
    str.append(") ");
    writeMsg(str);
}


//获取下一个nalu，包含了startCode
NALU *h264Parse::getNextNalu() {
    return getNalu();
}

void h264Parse::writeMsg(string msg) {
    fwrite(msg.c_str(), 1, msg.length(), h264OutF);
    fflush(h264OutF);
}

//每个nalu的size都是需要通过startcode来分割的。也就是只有一个个字节的遍历
void h264Parse::start() {
    int count = 0;
    string str;
    while (!feof(h264F)) {
        count++;
        NALU *na = getNalu();
        if (na == NULL) {
            LOGE(" WRONG ");
            break;
        }
        parseHeader(na->data, na->startCodeSize);
        str.append("| nal size ");
        str.append(numUtils->int2String(na->size));
        writeMsg(str);
        writeMsg("\n----------------------------------");
        str.clear();
        free(na->data);
        if (na->isEnd) {
            break;
        }
    }
}


h264Parse::~h264Parse() {
    if (h264F != NULL) {
        fclose(h264F);
    }
    if (h264OutF != NULL) {
        fclose(h264OutF);
    }
    if (numUtils != NULL) {
        delete numUtils;
    }
    LOGE(" DELTE H264PARSE ");
}
