
cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/jni/include)


add_library( # Sets the name of the library.
             my_ffmpeg

             SHARED

            src/main/jni/my_ffmpeg.cpp
            src/main/jni/decode_encode_test.cpp
            src/main/jni/my_open_sl_test.cpp
            src/main/jni/decode_show_gl.cpp
            src/main/jni/video_audio_decode_show.cpp
            src/main/jni/my_thread.cpp
            src/main/jni/my_c_plus_plus_thread.cpp
            src/main/jni/mysoundtouch.cpp
            src/main/jni/MyThread.cpp
            src/main/jni/FFmpegReadFrame.cpp
            src/main/jni/Utils.cpp
            src/main/jni/DecodeAudioThread.cpp
            src/main/jni/sonic.cpp
            src/main/jni/SonicRead.cpp
            src/main/jni/gpu_video_sl_audio.cpp
            src/main/jni/Mp4Player.cpp
            src/main/jni/ReadAvPacket.cpp
            src/main/jni/DecodeVideoThread.cpp
            src/main/jni/DecodeMyAudioThread.cpp
            src/main/jni/MyData.cpp
            src/main/jni/Notify.cpp
            src/main/jni/YuvPlayer.cpp
            src/main/jni/AudioPlayer.cpp
            )



add_library( avcodec-57 SHARED IMPORTED)
set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libavcodec-57.so)


add_library( avdevice-57 SHARED IMPORTED)
set_target_properties( avdevice-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libavdevice-57.so)


add_library( avfilter-6 SHARED IMPORTED)
set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libavfilter-6.so)


add_library( avformat-57 SHARED IMPORTED)
set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libavformat-57.so)


add_library( avutil-55 SHARED IMPORTED)
set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libavutil-55.so)

add_library( postproc-54 SHARED IMPORTED)
set_target_properties( postproc-54
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libpostproc-54.so)

add_library( swresample-2 SHARED IMPORTED)
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libswresample-2.so)


add_library( swscale-4 SHARED IMPORTED)
set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/${ANDROID_ABI}/libswscale-4.so)


target_link_libraries( # Specifies the target library.
                       my_ffmpeg

                       avcodec-57
                       avdevice-57
                       avfilter-6
                       avformat-57
                       avutil-55
                       postproc-54
                       swresample-2
                       swscale-4
                       OpenSLES

                      log )