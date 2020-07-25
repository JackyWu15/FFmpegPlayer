
cmake_minimum_required(VERSION 3.4.1)
include_directories(src/main/cpp/include)
include_directories(src/main/cpp/soundtouch/include)
add_library(
        native-lib

        SHARED

        src/main/cpp/ffplayer.cpp
        src/main/cpp/ffcallback.cpp
        src/main/cpp/ffmpeg.cpp
        src/main/cpp/ffaudio.cpp
        src/main/cpp/ffqueue.cpp
        src/main/cpp/ffbufferqueue.cpp
        src/main/cpp/pcmbean.cpp

        src/main/cpp/soundtouch/SoundTouch/AAFilter.cpp
        src/main/cpp/soundtouch/SoundTouch/FIFOSampleBuffer.cpp
        src/main/cpp/soundtouch/SoundTouch/FIRFilter.cpp
        src/main/cpp/soundtouch/SoundTouch/cpu_detect_x86.cpp
        src/main/cpp/soundtouch/SoundTouch/sse_optimized.cpp
        src/main/cpp/soundtouch/SoundTouch/RateTransposer.cpp
        src/main/cpp/soundtouch/SoundTouch/SoundTouch.cpp
        src/main/cpp/soundtouch/SoundTouch/InterpolateCubic.cpp
        src/main/cpp/soundtouch/SoundTouch/InterpolateLinear.cpp
        src/main/cpp/soundtouch/SoundTouch/InterpolateShannon.cpp
        src/main/cpp/soundtouch/SoundTouch/TDStretch.cpp
        src/main/cpp/soundtouch/SoundTouch/PeakFinder.cpp
)

#添加ffmpeg动态库进行关联
add_library( avcodec SHARED IMPORTED)
set_target_properties( avcodec
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so)

#add_library( avdevice-57 SHARED IMPORTED)
#set_target_properties( avdevice-57
#        PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavdevice-57.so)

add_library( avfilter SHARED IMPORTED)
set_target_properties( avfilter
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavfilter.so)


add_library( avformat SHARED IMPORTED)
set_target_properties( avformat
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavformat.so)


add_library( avutil SHARED IMPORTED)
set_target_properties( avutil
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libavutil.so)

#add_library( postproc-54 SHARED IMPORTED)
#set_target_properties( postproc-54
#        PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpostproc-54.so)

add_library( swresample SHARED IMPORTED)
set_target_properties( swresample
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswresample.so)


add_library( swscale SHARED IMPORTED)
set_target_properties( swscale
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libswscale.so)
find_library(
        log-lib

        log)

target_link_libraries(
        native-lib

        avcodec
#        avdevice-57
        avfilter
        avformat
        avutil
#        postproc-54
        swresample
        swscale

        OpenSLES

        android
        ${log-lib})