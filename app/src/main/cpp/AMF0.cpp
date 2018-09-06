//
// Created by Administrator on 2018/9/6/006.
//

#include "AMF0.h"

AMF0* AMF0::instance = NULL;

AMF0::AMF0(){

}

string AMF0::getAMF0(char amfType ,char *bytes , int start , int size){
    string str ;


    return str;
}


AMF0* AMF0::getInstance(){
    if(instance ==  NULL){
        instance = new AMF0();
    }
    return instance;
}

int AMF0::readNum(char *bytes , int start , int size){
    char type = bytes[start];
    switch (type){
        case kNumber:
            return 8;
        case kBoolean:
            break;

    }
    return 1;
}


AMF0::~AMF0(){

}