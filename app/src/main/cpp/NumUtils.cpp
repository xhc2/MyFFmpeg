//
// Created by Administrator on 2018/9/6/006.
//

#include "NumUtils.h"
#include "my_log.h"
//初始化
NumUtils* NumUtils::numUtils = NULL;

int NumUtils::array2Int(char *array , int start , int size){
    char temp[size];
    int tempCount = 0 ;
    int result = 0;
    int copySize = size;
    for(int i = size + start - 1  ; i >= start  ; -- i){
        if(array[i] != 0){
            temp[tempCount] = array[i];
            tempCount ++ ;
        }
        else{
            copySize -- ;
        }
    }
    memcpy(&result , temp , copySize);
    return result;
}

//这里不加static
NumUtils* NumUtils::getInstance(){
        if(numUtils == NULL){
            numUtils = new NumUtils();
        }
    return numUtils;
}

//读取字节
void NumUtils::bitRead(int result){
    int temp = 1;
    string str ;
    for(int i = 0 ;i < 32 ; ++ i){
        int a = temp & result;
        result = result >> 1;
        str.append(int2String(a));
    }
    LOGE(" %s ", str.c_str());
}


string NumUtils::int2String(int num){
    stringstream stream;
    stream<<num;
    string str = stream.str();
    stream.clear();
    return str;
}


NumUtils::NumUtils(){

}

