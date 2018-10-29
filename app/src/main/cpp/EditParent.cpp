//
// Created by Administrator on 2018/10/25/025.
//

#include "EditParent.h"

void  custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp = fopen("sdcard/FFmpeg/ffmpeg_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
};