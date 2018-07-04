//
// Created by Administrator on 2018/7/3/003.
//

#ifndef MYFFMPEG_MYWINDOW_H
#define MYFFMPEG_MYWINDOW_H

#include <android/native_window_jni.h>

class MyWindow{

private :
    ANativeWindow *nwin ;
    int outWidth , outHeight;
public :
    MyWindow(ANativeWindow *nwin , int outWidth , int outHeight);
    ~MyWindow();
};

#endif //MYFFMPEG_MYWINDOW_H
