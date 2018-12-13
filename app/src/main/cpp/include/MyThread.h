//
// Created by dugang on 2018/6/29.
//

#ifndef MYFFMPEG_MYTHREAD_H
#define MYFFMPEG_MYTHREAD_H


#include <pthread.h>

extern "C"{
#include <libavutil/time.h>
};

class MyThread
{
private:
    pthread_t pid;
    char *threadName;
    static void * start_thread(void *arg);                                                       //静态成员函数
public:
    bool pause ;
    bool isExit ;
    int start();
    void stop();
    void setPause();
    void setPlay();
    void join();
    void setThreadName(const char* name);
    MyThread();
    MyThread(const char* name);
    ~MyThread();
    void  threadSleep(int mis);
    pthread_mutex_t mutex_pthread ;
    virtual void run() = 0;//基类中的虚函数要么实现，要么是纯虚函数（绝对不允许声明不实现，也不纯虚）
};



#endif //MYFFMPEG_MYTHREAD_H
