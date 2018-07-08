//
// Created by dugang on 2018/6/29.
//



#include <MyThread.h>


MyThread::MyThread() {
    pthread_mutex_init(&mutex_pthread , NULL);
//    mutex_pthread = PTHREAD_MUTEX_INITIALIZER;
    pause = false;
    isExit = false;
}

int MyThread::start()
{
    if(pthread_create(&pid,NULL,start_thread,(void *)this) != 0)                 //创建一个线程(必须是全局函数)
    {
        return -1;
    }
    return 0;
}

void MyThread::stop(){
    isExit = true;
}

void MyThread::threadSleep(int mis) {
    av_usleep(1000 * 10);
}

void* MyThread::start_thread(void *arg) //静态成员函数只能访问静态变量或静态函数，通过传递this指针进行调用
{
    MyThread *ptr = (MyThread *)arg;
    ptr->run();
    return 0;                                                                                             //线程的实体是run
}