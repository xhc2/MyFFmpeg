//
// Created by Administrator on 2018/9/6/006.
//

#include "NumUtils.h"
#include "my_log.h"
//初始化
NumUtils* NumUtils::numUtils = NULL;

int NumUtils::array2Int(char *array ,const int start ,const int size){

    char temp[size];
    int tempCount = 0 ;
    int result = 0;
    int copySize = size;
    int removeSize = 0;

    for(int i = start ;i < size + start ; ++ i){
        if(array[i] == 0){
            removeSize++;
        }
        else{
            break;
        }
    }
    copySize -= removeSize;
    char newArray[copySize];
    memcpy(newArray , array + removeSize + start, copySize);

    //高低翻转下
    for(int i = size - removeSize - 1  ; i >= 0  ; -- i){
        temp[tempCount] = newArray[i];
        tempCount ++ ;

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

string NumUtils::double2String(double num){
    stringstream stream;
    stream<<num;
    string str = stream.str();
    stream.clear();
    return str;
}

double NumUtils::array2Double(char *array , int start){

    double result = 0;
    int doubleSize = 8;
    char *resultArray = (char *)malloc(doubleSize);
    int j = start + doubleSize - 1 ;
    for(int i = 0 ;i < doubleSize ; ++ i ){
        resultArray[i] = array[j];
        -- j;

    }
    memcpy(&result , resultArray , doubleSize);

    return result;

}



NumUtils::NumUtils(){

}

