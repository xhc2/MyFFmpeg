#include <my_c_plus_plus_thread.h>
#include <thread>
#include <my_log.h>
#include <mutex>

using namespace std;

mutex my_mutex;
int num_count = 100;

void Sleep(int mis){
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

void run(const char* str){

    while(num_count > 0){
        my_mutex.lock();
        if(num_count > 0 ){
            num_count -- ;
            LOGE(" thread = %s , num = %d " , str , num_count);
            Sleep(1);
        }
        my_mutex.unlock();
    }
}

int initMyCPlusPlusThread() {
    num_count = 100;
    thread thread1 (run , "thread 1 ");
    // 将线程放到后台，但是就不好管理了。只有通过flag来处理停止，但是不能通过join来确切知道是什么时候线程执行完毕。
    thread1.detach();
    thread thread2 (run ,  "thread 2 ");
    thread2.detach();
//    join不能和detach一起使用，不然崩溃。
//    thread1.join();

    return 1;
}

int startMyCPlusPlusThread() {

    return 1;
}

int stopMyCPlusPlusThread() {

    return 1;
}

