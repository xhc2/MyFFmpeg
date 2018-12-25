//
// Created by Administrator on 2018/12/25/025.
//

/**
 * http://kns.cnki.net/KCMS/detail/detail.aspx?dbcode=CJFQ&dbname=CJFDHIS2&filename=BJGB201306010&uid=WEEvREcwSlJHSldRa1FhdkJkVWEyd2MwSXVScUc0anhZUmZvN093NU5IUT0=$9A4hF_YAuvQ5obgVAqNKPCYcEjKensW4IQMovwHtwkF4VYPoHbKxJw!!&v=MTI5NTVoVzd6Skp5Zk1iTEc0SDlMTXFZOUVaSVI4ZVgxTHV4WVM3RGgxVDNxVHJXTTFGckNVUkxLZVplUnFGeXo=
 * 期刊
 *
 * http://kreader.cnki.net/Kreader/CatalogViewPage.aspx?dbCode=cdmd&filename=1015708671.nh&tablename=CMFD201601&compose=&first=1&uid=WEEvREcwSlJHSldRa1FhdXNXa0d1ZzVBcWM1cTVFMjJVL2tHVENYYkFlVT0=$9A4hF_YAuvQ5obgVAqNKPCYcEjKensW4IQMovwHtwkF4VYPoHbKxJw!!
 * 协议相关
 */

#include "MyRtmp.h"
#include <sys/socket.h>"
#include <my_log.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>

/**
 * 简单实现rtmp协议。
 * 参照开发文档。学习别人代码。
 * 1.rtmp连接
 * 2.发送一段h264流
 * 3.发送一段aac流
 * 4.发送一段信息流
 * 5.释放连接
 * @param playPath
 */

struct RtmpHeader{
    char format ; //1 B
    char csId ; // 1B
    int32_t timeStamp ; // 3 B
    int32_t bodySize ; // 3 B
    char typeId ; // 1 B
    int32_t streamId; // 4 B
};

struct RtmpMsg{
    RtmpHeader header;
    char *body ;
};

struct sockaddr_in server_addr;
int sk;

MyRtmp::MyRtmp() {

}

int MyRtmp::rtmpConnect() {
    socketCreate();
//    char *msg = " xhc test socekt ";
//    int ret = send(sk, msg , strlen(msg), 0);
//    if(ret < 0){
//        LOGE(" SEND FAILD  !");
//    }

    return 1;
}

int MyRtmp::socketCreate() {
    sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        LOGE(" SCOKET FAILD !");
        return -1;
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, addr, &server_addr.sin_addr) < 0) {    //设置ip地址
        LOGI("address error");
        return -1;
    }
    int connfd = connect(sk, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connfd < 0) {
        return -1;
    }
    return 1;
}

int MyRtmp::socketClose() {
    /**
    * SHUT_RD 关闭读取操作，但是可以写
    * SHUT_WR 关闭写操作，但是可以读取
    * SHUT_RDWR  相当于调用shutdown两次：首先是以SHUT_RD,然后以SHUT_WR
    */
    if (sk >= 0) {
        shutdown(sk, SHUT_RDWR);
    }
    return 1;
}


int MyRtmp::rtmpHandShake() {
    int ret;
    char *c0c1 = (char *) malloc(1537); // c0+c1

    for (int i = 0; i < 1537; ++i) {
        c0c1[i] = (char) i;
    }
    // version
    c0c1[0] = 3;
    //time
    int32_t time32 = time(NULL);
    char *nowTime = (char *) &time32;
    c0c1[1] = nowTime[3];
    c0c1[2] = nowTime[2];
    c0c1[3] = nowTime[1];
    c0c1[4] = nowTime[0];
    //zero
    c0c1[5] = 0x00;
    c0c1[6] = 0x00;
    c0c1[7] = 0x00;
    c0c1[8] = 0x00;

    send(sk, c0c1, 1537, 0); // c0+c1

    char *s0s1s2 = (char *) malloc(3073); //s0+s1+s2

    ret = recvFull(s0s1s2, 3073);
    if(ret < 0){
        LOGE(" handshake s0s1s2 faild !");
        return -1;
    }
    char *c2 = (char *)malloc(1536);
    time32 = time(NULL);
    nowTime = (char *) &time32;
    c2[0] = nowTime[3];
    c2[1] = nowTime[2];
    c2[2] = nowTime[1];
    c2[3] = nowTime[0];
    c2[4] = 0x00;
    c2[5] = 0x00;
    c2[6] = 0x00;
    c2[7] = 0x00;
    memcpy(c2 + 7 , c0c1 + 8 , 1536);
    send(sk , c2 , 1536 , 0 );
    LOGE(" handshake success ");
    return 1;
}

//连接rtmp
int MyRtmp::connectApp() {

    RtmpMsg *rmConnectLive = new RtmpMsg();
    RtmpHeader rmLiveHeader = rmConnectLive->header ;
    rmLiveHeader.format = 0 ;
    rmLiveHeader.csId = 3;



    RtmpMsg *rmWA = new RtmpMsg();
    RtmpHeader rh = rmWA->header;
    rh.format = 0;
    rh.csId = 2;
    rh.timeStamp = 0;
    rh.bodySize = 4;
    rh.typeId = 5;
    rh.streamId = 0;
    rmWA->body = (char *)malloc(4);
    int32_t size = 2500000;
    memcpy(rmWA->body , &size , 4);

    char *chunk = (char *)malloc(13 + 4);
    memcpy(chunk , &rh.format , 1);
    memcpy(chunk + 1 , &rh.csId , 1);
    memcpy(chunk + 2 , &rh.timeStamp , 3);
    memcpy(chunk + 5 , &rh.bodySize , 3);
    memcpy(chunk + 8 , &rh.typeId , 1);
    memcpy(chunk + 9 , &rh.streamId , 4);
    memcpy(chunk + 13 , rmWA->body , 4);
    send(sk , chunk , 16 , 0) ;
//    send();
    return 1;
}




//需要从tcp缓冲区中读取多少个字节
int MyRtmp::recvFull(char *dst, int size) {
    int offset = 0;
    int ret;
    char *buffer = dst + offset;
    while (offset < size) {
        ret = recv(sk, buffer, size, 0);
        if (ret < 0) {
            LOGE(" RECV ERROR %d ", ret);
            break;
        }
        offset += ret;
        buffer = dst + offset;
        LOGE(" OFFSET %d ", offset);
    }

    return offset;
}


int MyRtmp::startRtmp() {
    rtmpConnect();
    rtmpHandShake();
    connectApp();
    return 1;
}


MyRtmp::~MyRtmp() {
    socketClose();
}



