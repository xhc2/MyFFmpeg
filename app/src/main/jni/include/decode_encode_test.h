//
// Created by Administrator on 2018/4/21/021.
//

#ifndef MYFFMPEG_DECODE_ENCODE_TEST_H
#define MYFFMPEG_DECODE_ENCODE_TEST_H
int decode(const char* input_path ,JNIEnv* env, jobject surface);

int encode(const char* input_yuv , const char* input_pcm);
#endif //MYFFMPEG_DECODE_ENCODE_TEST_H
