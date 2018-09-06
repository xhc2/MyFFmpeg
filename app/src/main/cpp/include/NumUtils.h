//
// Created by Administrator on 2018/9/6/006.
//



#ifndef MYFFMPEG_NUMUTILS_H
#define MYFFMPEG_NUMUTILS_H
#include <string>
#include <stdio.h>
#include <sstream>
using namespace std;
class NumUtils{

private :
    NumUtils();
    static NumUtils* numUtils;

//    static pthread_mutex_t mutex_pthread ;
public :
    static NumUtils* getInstance();
    int array2Int(char *array , int start , int size);
    int array2Double(char *array);
    string int2String(int num);
    void bitRead(int result);
};

#endif //MYFFMPEG_NUMUTILS_H
