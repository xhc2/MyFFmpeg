


#ifndef MYFFMPEG_CALLJAVA_H
#define MYFFMPEG_CALLJAVA_H

#include "jni.h"
class CallJava{

private :
    jmethodID jmethodID1;
    JNIEnv *env;
    jclass type;
public :
    CallJava(JNIEnv *env , jclass type);
    ~CallJava();
    void callStr(char *msg);
};

#endif //MYFFMPEG_CALLJAVA_H
