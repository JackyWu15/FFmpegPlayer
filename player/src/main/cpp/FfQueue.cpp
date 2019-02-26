//
// 用于存储音频数据的队列
// Created by Administrator on 2019/2/20.
//

#include "FfQueue.h"
FfQueue::FfQueue() {
    pthread_mutex_init(&queueMutex,NULL);
    pthread_cond_init(&queueCond,NULL);
}

FfQueue::~FfQueue() {

}

void FfQueue::pushAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&queueMutex);
    avPacketQueue.push(avPacket);
    if(LOGDEBUG){
        LOGI("push the AVPacket,and size is %d",avPacketQueue.size());
    }
    pthread_cond_signal(&queueCond);
    pthread_mutex_unlock(&queueMutex);
}

int FfQueue::popAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&queueMutex);

    if(this->ffPlayStatus==1){
        while (true){
            if(avPacketQueue.size()>0){
                AVPacket * pPacket = avPacketQueue.front();
                if(av_packet_ref(avPacket,pPacket)==0){
                    avPacketQueue.pop();
                }
                av_packet_free(&pPacket);
                av_free(pPacket);
                pPacket = NULL;
                if(LOGDEBUG){
                    LOGI("pop the AVPacket,and size is %d",avPacketQueue.size())
                }
                break;
            } else{
                pthread_cond_wait(&queueCond,&queueMutex);
            }
        }
    }
    pthread_mutex_unlock(&queueMutex);

    return 0;
}

void FfQueue::setFfPlayStatus(int ffPlayStatus) {
    this->ffPlayStatus = ffPlayStatus;

}

int FfQueue::getQueueSize() {
    pthread_mutex_lock(&queueMutex);
    int queueSize = this->avPacketQueue.size();
    pthread_mutex_unlock(&queueMutex);
    return queueSize;
}


