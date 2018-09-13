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
    u_int u(char *bs, char bitCount);
//    static pthread_mutex_t mutex_pthread ;
public :
    static NumUtils* getInstance();
    int array2Int(char *array , int start , int size);
    double array2Double(char *array , int start);
    //哥伦布编码
    u_int columbusCoding(char *param);
    string int2String(int num);
    string double2String(double num);
    void bitRead(int result);
};

#endif //MYFFMPEG_NUMUTILS_H
