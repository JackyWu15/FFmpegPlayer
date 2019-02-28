//
// Created by Administrator on 2019/2/20.
//

#ifndef FFMPEGPLAYER_FFQueue_H
#define FFMPEGPLAYER_FFQueue_H
#include "queue"
#include "pthread.h"
#include "unistd.h"
#include "androidlog.h"
using namespace std;
extern "C"{
#include "libavcodec/avcodec.h"
};


class FFQueue {
public:
    queue<AVPacket*> avPacketQueue;
    pthread_mutex_t queueMutex;
    pthread_cond_t queueCond;
    int ffPlayStatus;
public:
    FFQueue();
    ~FFQueue();
public:
    void setFfPlayStatus(int ffPlayStatus);
    void pushAVPacket(AVPacket* avPacket);
    int popAVPacket(AVPacket* avPacket);
    int getQueueSize();

};


#endif //FFMPEGPLAYER_FFQueue_H
