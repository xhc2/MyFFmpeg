#include <my_c_plus_plus_thread.h>
#include <thread>
#include <my_log.h>
/**
 * 需要测试同步，阻塞，销毁等
 */
using namespace std;

void Sleep(int mis){
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

void run(){
    for(int i = 0 ; i < 10 ; ++i){
        LOGE("I = %d " , i);
        Sleep(1000);
    }
}

int initMyCPlusPlusThread() {
    thread thread1 (run);
    thread1.detach();
    return 1;
}

int startMyCPlusPlusThread() {

    return 1;
}

int stopMyCPlusPlusThread() {

    return 1;
}

