//
// Created by Administrator on 2018/12/25/025.
//

#ifndef MYFFMPEG_MYRTMP_H
#define MYFFMPEG_MYRTMP_H

class MyRtmp{

private :
    int port = 1935 ;
    const char* addr = "192.168.15.237";
public :
    MyRtmp();
    int rtmpConnect();
    int startRtmp();
    int socketCreate();
    int socketClose();
    int rtmpHandShake();
    ~MyRtmp();
};

#endif //MYFFMPEG_MYRTMP_H
