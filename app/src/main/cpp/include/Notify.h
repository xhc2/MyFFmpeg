//
// Created by dugang on 2018/7/4.
//

#ifndef MYFFMPEG_NOTIFY_H
#define MYFFMPEG_NOTIFY_H

#include <list>
#include <vector>
#include "my_data.h"

using namespace std;
class Notify{

private :
    vector<Notify*> listN;

public :

    void notify(MyData *mydata);
    void addNotify(Notify *n);
    void removeNotify();
    ~Notify();
    virtual void update(MyData *mydata) = 0;
};

#endif //MYFFMPEG_NOTIFY_H
