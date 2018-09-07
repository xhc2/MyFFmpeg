//
// Created by Administrator on 2018/9/6/006.
//
#include<string>
using namespace std;
#ifndef MYFFMPEG_AMF0_H
#define MYFFMPEG_AMF0_H
enum amf0{
    kNumber=0x00 ,
    kBoolean ,
    kString ,
    kObject ,
    kMovieClip ,
    kNull ,
    kUndefined ,
    kReference ,
    kEcmaArray ,
    kEndOfObject ,
    kArray ,
    kDate ,
};
class AMF0{

private :

    static AMF0* instance;
    AMF0();
public :
    static AMF0* getInstance();

    //得到amf中的内容，因为用于打印，直接返回string
    string getAMF0(char amfType ,char *bytes , int start , int size);
    //根据amf的类型，返回还需要读取多少字节
    int readNum(char *bytes , int start , int size);


    ~AMF0();



};
#endif //MYFFMPEG_AMF0_H
