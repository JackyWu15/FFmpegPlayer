//
// Created by Administrator on 2019/2/18.
//

#ifndef FFmpegPLAYER_FFmpeg_H
#define FFmpegPLAYER_FFmpeg_H

#include "ffcallback.h"
#include "pthread.h"
#include "ffaudio.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};

class FFmpeg {
public:
    const char* filePath = NULL;
    FFCallBack* ffCallBack = NULL;
    pthread_t decodeThread;
    AVFormatContext* avFormatContext = NULL;
    FFAudio* ffAudio = NULL;
    pthread_mutex_t playMutex;
    int ffPlayStatus ;
    pthread_mutex_t seekMutex;

    int videoIndex;
    AVCodecParameters *parameters;
    AVCodec *avCodec;
    AVCodecContext *avCodecContext;
    AVPacket * avPacket;
public:
    FFmpeg(FFCallBack *ffCallBack, const char* filePath);
    ~FFmpeg();

public:
    void prepare();//准备
    void decodeAudio();//初始化解码器并打开流
    void start();//启动
    void pause();//暂停
    void play();//播放
    void release();
    void seek(int64_t seconds);


    void video_prepare();
    void video_start(JNIEnv *env, jobject surface);
};


#endif //FFmpegPLAYER_FFmpeg_H
