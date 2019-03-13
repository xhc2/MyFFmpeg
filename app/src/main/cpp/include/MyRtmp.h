//
// Created by Administrator on 2018/12/25/025.
//

#ifndef MYFFMPEG_MYRTMP_H
#define MYFFMPEG_MYRTMP_H

class MyRtmp{

private :
    int port = 1935 ;
    const char* addr = "192.168.20.142";
public :
    MyRtmp();
    int rtmpConnect();
    int startRtmp();
    int socketCreate();
    int socketClose();
    int rtmpHandShake();
    int connectApp();
    int recvFull(char *buffer , int size);
    ~MyRtmp();
};

#endif //MYFFMPEG_MYRTMP_H
