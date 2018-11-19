//
// Created by dugang on 2018/6/29.
//



#include <MyThread.h>
#include <my_log.h>


MyThread::MyThread() {
    pthread_mutex_init(&mutex_pthread, NULL);
    pause = false;
    isExit = false;
    pid = NULL;
    threadName = NULL;
}

MyThread::MyThread(const char *name) {
    pthread_mutex_init(&mutex_pthread, NULL);
    pause = false;
    isExit = false;
    pid = NULL;
    int len = strlen(name);
    len++;
    threadName = (char *) malloc(len);
    strcpy(threadName, name);
}

void MyThread::setThreadName(const char *name) {
    int len = strlen(name);
    len++;
    threadName = (char *) malloc(len);
    strcpy(threadName, name);
}

int MyThread::start() {
    isExit = false;
    if (pthread_create(&pid, NULL, start_thread, (void *) this) !=
        0)                 //创建一个线程(必须是全局函数)
    {
        return -1;
    }
    return 0;
}

void MyThread::stop() {
    if (threadName != NULL) {
        LOGE(" THREAD STOP %s ", threadName);
    }

    isExit = true;
}

void MyThread::threadSleep(int mis) {
    av_usleep(1000 * mis);
}

void MyThread::join() {
    if (threadName != NULL) {
        LOGE(" thread_join  %s ", threadName);
    }
    if (pid == NULL) {
        LOGE("join thread faild ! may be thread not started !");
        return;
    }
    void *t;
    pthread_join(pid, &t);
}

void MyThread::setPause() {
    pause = true;
}

void MyThread::setPlay() {
    pause = false;
}


void *MyThread::start_thread(void *arg) //静态成员函数只能访问静态变量或静态函数，通过传递this指针进行调用
{
    MyThread *ptr = (MyThread *) arg;
    ptr->run();
    return 0;   //线程的实体是run
}

MyThread::~MyThread() {
    LOGE(" DESTROY THREAD ");
    pthread_mutex_destroy(&mutex_pthread);
    if (threadName != NULL) {
        free(threadName);
    }

}