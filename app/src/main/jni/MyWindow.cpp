//
// Created by Administrator on 2018/7/3/003.
//

#include "MyWindow.h"


MyWindow::MyWindow(ANativeWindow *nwin, int outWidth , int outHeight){
    this->outWidth = outWidth;
    this->outHeight = outHeight;
    this->nwin = nwin;
}

MyWindow::~MyWindow(){

}
