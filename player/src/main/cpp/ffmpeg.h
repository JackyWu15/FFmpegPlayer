//
// Created by Administrator on 2019/2/18.
//

#ifndef FFmpegPLAYER_FFmpeg_H
#define FFmpegPLAYER_FFmpeg_H

#include "ffcallback.h"
#include "pthread.h"
#include "ffaudio.h"
extern "C"{
#include "libavformat/avformat.h"
};

class FFmpeg {
public:
    const char* filePath = NULL;
    FFCallBack* ffCallBack = NULL;
    pthread_t decodeThread;
    AVFormatContext* avFormatContext = NULL;
    FFAudio* ffAudio = NULL;

public:
    FFmpeg(FFCallBack *ffCallBack, const char* filePath);
    ~FFmpeg();

public:
    void prepare();//准备
    void decodeAudio();//初始化解码器并打开流
    void start();//启动
    void pause();//暂停
    void play();//播放
};


#endif //FFmpegPLAYER_FFmpeg_H
