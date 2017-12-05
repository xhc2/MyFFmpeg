//
// Created by dugang on 2017/12/4.
//

#ifndef MYFFMPEG_MY_MUXER_H
#define MYFFMPEG_MY_MUXER_H

int demuxer(const char* input_path , const char *out_v_path , const char *out_a_path);
int muxer(const char* output_path , const char *input_v_path , const char *input_a_path);
#endif //MYFFMPEG_MY_MUXER_H
