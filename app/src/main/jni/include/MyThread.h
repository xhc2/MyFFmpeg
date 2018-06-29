//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_MYTHREAD_H
#define MYFFMPEG_MYTHREAD_H


#include <pthread.h>
#include <thread>

using namespace std;

class MyThread
{
private:
    pthread_t pid;

private:
    static void * start_thread(void *arg);                                                       //静态成员函数
public:
    bool pause = false;
    bool isExit = false;
    int start();
    void  threadSleep(int mis);
    virtual void run() = 0;//基类中的虚函数要么实现，要么是纯虚函数（绝对不允许声明不实现，也不纯虚）
};



#endif //MYFFMPEG_MYTHREAD_H
