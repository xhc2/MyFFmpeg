//
// Created by dugang on 2018/4/27.
//

#include "my_thread.h"
#include "pthread.h"
#include "my_log.h"
#include "stdio.h"
#include <thread>

using namespace std;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num = 0 ;
void addNum(){
//    pthread_mutex_lock(&mutex);
    num ++;
    LOGE(" NUM = %d " , num);
//    pthread_mutex_unlock(&mutex);
}


void ThreadSleep_gpu2(int mis) {
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}


//线程执行函数
void* run(void *arg){

    for(int i = 0; i < 50  ; ++i){
        LOGE(" THREAD RUN ! %s " , (char *)arg);
        ThreadSleep_gpu2(1000);
        addNum();
    }
    return (void*)1;//返回的是一个指针。
}

int initThread(){
    int result;
    num = 0 ;
    pthread_t pthread1;
    pthread_t pthread2;

    result = pthread_create( &pthread1 , NULL , run , (void *)"thread  1");

//    if(result == 0){
//        LOGE(" pthread_create SUCCESS ! ");
//        // 等待线程tid执行结束
//        void* t;
//        result = pthread_join(pthread1, &t);
//        if(result != 0){
//            return RESULT_FAILD;
//        }
//        if(t != NULL ){
//            LOGE(" THREAD RESULT %d " , t);
//        }
//        return RESULT_SUCCESS;
//    }

    result = pthread_create( &pthread2 , NULL , run , (void *)"thread  2");
//    if(result == 0){
//        LOGE(" pthread_create SUCCESS ! ");
//        // 等待线程tid执行结束
//        void* t;
//        result = pthread_join(pthread2, &t);
//        if(result != 0){
//            return RESULT_FAILD;
//        }
//        if(t != NULL ){
//            LOGE(" THREAD RESULT %d " , t);
//        }
//        return RESULT_SUCCESS;
//    }

    LOGE(" pthread_create RESULT_FAILD ! ");
    return RESULT_FAILD;
}



int threadFree(){

    return 1;
}

int threadRun(){

    return 1;
}