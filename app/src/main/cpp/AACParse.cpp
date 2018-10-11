//
// Created by Administrator on 2018/10/10/010.
//


#include "aacparse.h"


AACParse::AACParse(const char *path) {
    int len = strlen(path);
    this->path = (char *) malloc(len);
    memcpy(this->path, path, len);
    LOGE(" path %s ", path);
    aacF = fopen(this->path, "r");

}

AACFrame *AACParse::getAACFrame(bool write) {
    AACFrame *frame = (AACFrame *) malloc(sizeof(AACFrame));
    frame->size = 0;
    int size = parseAACHeader(write);
    if(size == -1){
        return NULL;
    }
    frame->size = size;
    frame->data = (char *)malloc(size);
    fread(frame->data , 1, size , aacF);

    return frame;
}


int AACParse::parseAACHeader(bool write) {
    char *head = (char *) malloc(7);
    FrameHead fh;
    fread(head, 1, 7, aacF);
    fseek(aacF , -7 , 1);
    fh.syncword = head[0];
    fh.syncword <<= 8;
    fh.syncword = fh.syncword | head[1];
    LOGE(" SYN %x " , fh.syncword);
    if(( fh.syncword & 0xfff0 ) != 0xfff0){
        LOGE(" syncword faild  ");
        return -1;
    }
    int frameLength = 0 ;
    frameLength = head[3] & 0x03;
    frameLength <<= 8;
    frameLength |= head[4];
    frameLength <<= 3;
    frameLength |= ((head[5] & 0xe0) >> 5);
    LOGE(" frameLength %d " , frameLength);
    if (write) {
        FILE *outFile = fopen("sdcard/FFmpeg/parseAAC.txt", "wb+");

        fclose(outFile);
    }

    free(head);
    return frameLength;
}

void AACParse::parseStart() {
    getAACFrame(false);
}

AACParse::~AACParse() {

}