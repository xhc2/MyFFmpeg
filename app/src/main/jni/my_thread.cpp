//
// Created by dugang on 2018/4/27.
//

#include "my_thread.h"
#include "pthread.h"
#include "my_log.h"
#include "stdio.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//线程执行函数
void* run(void *arg){

    for(int i = 0; i < 1000  ; ++i){
        LOGE(" THREAD RUN ! %s " , (char *)arg);
        sleep(10);
    }
    return (void*)1;//返回的是一个指针。
}

int initThread(){
    int result;


    result = pthread_mutex_init(&mutex , NULL);
    LOGE(" MUTEX %d " , result);
    if(result != 0){
        LOGE(" MUTEX fiald ！ ");
        return RESULT_FAILD;
    }

    pthread_t pthread1;
    pthread_t pthread2;

    result = pthread_create( &pthread1 , NULL , run , (void *)"thread  1");
    if(result == 0){
        LOGE(" pthread_create SUCCESS ! ");
        // 等待线程tid执行结束
        void* t;
        result = pthread_join(pthread1, &t);
        if(result != 0){
            return RESULT_FAILD;
        }
        if(t != NULL ){
            LOGE(" THREAD RESULT %d " , t);
        }
        return RESULT_SUCCESS;
    }

    result = pthread_create( &pthread2 , NULL , run , (void *)"thread  2");
    if(result == 0){
        LOGE(" pthread_create SUCCESS ! ");
        // 等待线程tid执行结束
        void* t;
        result = pthread_join(pthread2, &t);
        if(result != 0){
            return RESULT_FAILD;
        }
        if(t != NULL ){
            LOGE(" THREAD RESULT %d " , t);
        }
        return RESULT_SUCCESS;
    }

    LOGE(" pthread_create RESULT_FAILD ! ");
    return RESULT_FAILD;
}

int threadFree(){

    return 1;
}

int threadRun(){

    return 1;
}