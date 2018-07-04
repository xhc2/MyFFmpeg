//
// Created by dugang on 2018/7/4.
//

#include "Notify.h"

void Notify::addNotify(Notify *n){
    listN.push_back(n);
}

void Notify::notify(MyData mydata) {
    for(int i = 0 ;i < listN.size() ; ++ i){
        listN.at(i)->update(mydata);
    }
}