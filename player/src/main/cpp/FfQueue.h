//
// Created by Administrator on 2019/2/20.
//

#ifndef FFMPEGPLAYER_FFQUEUE_H
#define FFMPEGPLAYER_FFQUEUE_H
#include "queue"
#include "pthread.h"
#include "unistd.h"
#include "AndroidLog.h"
using namespace std;
extern "C"{
#include "libavcodec/avcodec.h"
};


class FfQueue {
public:
    queue<AVPacket*> avPacketQueue;
    pthread_mutex_t queueMutex;
    pthread_cond_t queueCond;
    int ffPlayStatus;
public:
    FfQueue();
    ~FfQueue();
public:
    void setFfPlayStatus(int ffPlayStatus);
    void pushAVPacket(AVPacket* avPacket);
    int popAVPacket(AVPacket* avPacket);
    int getQueueSize();

};


#endif //FFMPEGPLAYER_FFQUEUE_H
