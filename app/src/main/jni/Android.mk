LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := prebuilt/libavcodec.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := prebuilt/libavfilter.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := prebuilt/libavformat.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := prebuilt/libavutil.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := prebuilt/libswresample.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := prebuilt/libswscale.so
include $(PREBUILT_SHARED_LIBRARY)

#soundtouch
include $(CLEAR_VARS)
LOCAL_MODULE := soundtouch
LOCAL_SRC_FILES := prebuilt/libsoundtouch.so
include $(PREBUILT_SHARED_LIBRARY)

# Program FFmpeg

include $(CLEAR_VARS)
LOCAL_MODULE := my_ffmpeg
LOCAL_SRC_FILES := my_ffmpeg.cpp decode_encode_test.cpp my_open_sl_test.cpp \
                    decode_show_gl.cpp video_audio_decode_show.cpp my_thread.cpp \
                    my_c_plus_plus_thread.cpp gpu_video_sl_audio.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

NDK_TOOLCHAIN_VERSION=4.8
LOCAL_LDLIBS := -llog -lz -landroid -lOpenSLES -lGLESv2 -lEGL
LOCAL_SHARED_LIBRARIES := soundtouch avfilter avformat avutil swresample swscale avcodec my_ffmpeg


#LOCAL_SANITIZE:=unsigned-integer-overflow signed-integer-overflow
#LOCAL_SANITIZE_DIAG:=unsigned-integer-overflow signed-integer-overflow
#LOCAL_CLANG:=true
#LOCAL_SANITIZE:=address
#LOCAL_MODULE_RELATIVE_PATH := asan

#LOCAL_CFLAGS += -std=c++11
LOCAL_CFLAGS := -std=c++11 -Wall -Werror -O0
LOCAL_SANITIZE := address
LOCAL_MODULE_RELATIVE_PATH := asan
LOCAL_CLANG:=true
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
#LOCAL_SANITIZE_DIAG := alignment bounds null unreachable integer

include $(BUILD_SHARED_LIBRARY)
