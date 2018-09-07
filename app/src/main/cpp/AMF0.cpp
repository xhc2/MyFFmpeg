//
// Created by Administrator on 2018/9/6/006.
//

#include <NumUtils.h>
#include <my_log.h>
#include "AMF0.h"

AMF0 *AMF0::instance = NULL;

AMF0::AMF0() {

}

string AMF0::getAMF0(char amfType, char *bytes, int start, int size) {
    string *str;
    char *content = (char *) malloc(size);
    memcpy(content, bytes + start, size);

    switch (amfType) {
        case kNumber:
            break;
        case kBoolean:
            return content[0] == 1 ? "true" : "false";
        case kString: {
            //这里用括号是因为case中不能有 变量初始化
            char *stringContent = (char *) malloc(size + 1);
            memcpy(stringContent, content, size);
            stringContent[size] = '\0';
            str = new string(stringContent);
            free(stringContent);
        }

            break;
        case kObject:

            break;
        case kEcmaArray:

            break;
    }
    free(content);

    return *str;
}


AMF0 *AMF0::getInstance() {
    if (instance == NULL) {
        instance = new AMF0();
    }
    return instance;
}

//这个里面byte的第一个字节必须存type
int AMF0::readNum(char *bytes, int start, int size) {
    char type = bytes[start];
    LOGE(" TYPE %d " , type);
    switch (type) {
        case kNumber:
            return 8;
        case kBoolean:

            return 1;
        case kString:

            return NumUtils::getInstance()->array2Int(bytes, start + 1, 2);

        case kObject:

            break;
        case kEcmaArray:
            //+1 是因为第一个是type ，size目前我认为是固定的。
            return NumUtils::getInstance()->array2Int(bytes, start + 1, 4);
    }
    return 1;
}


AMF0::~AMF0() {

}