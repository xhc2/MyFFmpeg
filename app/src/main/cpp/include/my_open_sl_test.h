//
// Created by Administrator on 2018/4/23/023.
//

#ifndef MYFFMPEG_MY_OPEN_SL_TEST_H
#define MYFFMPEG_MY_OPEN_SL_TEST_H
#include <queue>

void createEngine();
void createBufferQueueAudioPlayer();
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
void startPlayTest();
#endif //MYFFMPEG_MY_OPEN_SL_TEST_H
