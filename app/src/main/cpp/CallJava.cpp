//
// Created by dugang on 2018/7/23.
//

#include <my_log.h>
#include "CallJava.h"

CallJava::CallJava(JNIEnv *env  , jclass type){
    this->env = env;
    this->type = type;
    //第三个参数是方法的签名。返回值和参数都在里面包含
    jmethodID1 = env->GetStaticMethodID(type , "nativeNotify" , "(Ljava/lang/String;)V");
    LOGE(" jmethodID1 %d " , (jmethodID1 == NULL));
}


CallJava::~CallJava(){

}


void CallJava::callStr(char *msg){
    if(jmethodID1 != NULL){
        jstring msgStr = env->NewStringUTF(msg);
        env->CallStaticVoidMethod(type , jmethodID1 , msgStr);
    }

}