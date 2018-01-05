
#include "my_ffmpeg.h"
#include "My_LOG.h"

char *outPath;

void init(char *ouputPath){
    outPath = ouputPath;
    LOGE(" INIT ...%s" , outPath);
}
void close(){
    LOGE(" CLOSE ...");
}

void encodeCamera(){
    LOGE(" encodeCamera ...");
}


