//
// Created by Administrator on 2019/2/18.
//

#ifndef FFMPEGPLAYER_FFMPEG_H
#define FFMPEGPLAYER_FFMPEG_H

#include "FfCallBcak.h"
#include "pthread.h"
#include "FfAudio.h"
extern "C"{
#include "libavformat/avformat.h"
};

class Ffmpeg {
public:
    const char* filePath = NULL;
    FfCallBack* ffCallBack = NULL;
    pthread_t decodeThread;
    pthread_mutex_t decodeMutex;
    AVFormatContext* avFormatContext = NULL;
    AVCodecParameters* avCodecParameters = NULL;
    FfAudio* ffAudio = NULL;

public:
    Ffmpeg(FfCallBack *ffCallBack, const char* filePath);
    ~Ffmpeg();

public:
    void prepare();
    void decodeAudio();//初始化解码器并打开流
    void start();

};


#endif //FFMPEGPLAYER_FFMPEG_H
