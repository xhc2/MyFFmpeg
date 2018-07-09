
cmake_minimum_required(VERSION 3.4.1)

include_directories(include)


add_library(
            my_ffmpeg
            SHARED

             sonic.cpp
             SonicRead.cpp
             Utils.cpp
             MyThread.cpp
            my_ffmpeg.cpp
            Mp4Player.cpp
            ReadAvPacket.cpp
            DecodeVideoThread.cpp
            DecodeMyAudioThread.cpp
            MyData.cpp
            Notify.cpp
            YuvPlayer.cpp
            AudioPlayer.cpp
            )



add_library( avcodec-57 SHARED IMPORTED)
set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libavcodec-57.so)


add_library( avfilter-6 SHARED IMPORTED)
set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libavfilter-6.so)


add_library( avformat-57 SHARED IMPORTED)
set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libavformat-57.so)


add_library( avutil-55 SHARED IMPORTED)
set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libavutil-55.so)


add_library( swresample-2 SHARED IMPORTED)
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libswresample-2.so)


add_library( swscale-4 SHARED IMPORTED)
set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/prebuilt/x86/libswscale-4.so)


add_library( avdevice-57 SHARED IMPORTED)
set_target_properties( avdevice-57
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavdevice-57.so)


add_library( postproc-54 SHARED IMPORTED)
set_target_properties( postproc-54
                       PROPERTIES IMPORTED_LOCATION
                       ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-54.so)

target_link_libraries( # Specifies the target library.
                       my_ffmpeg
                       avcodec-57
                       avfilter-6
                       avformat-57
                       avutil-55
                       swresample-2
                       swscale-4
                       avdevice-57
                       postproc-54
                       OpenSLES
                       EGL
                       GLESv2
                       android
                       log )