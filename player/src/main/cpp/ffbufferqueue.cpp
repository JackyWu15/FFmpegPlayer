//
// Created by ywl on 2017-12-3.
//

#include "ffbufferqueue.h"
#include "AndroidLog.h"

FFBufferQueue::FFBufferQueue() {
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);
}

FFBufferQueue::~FFBufferQueue() {
    pthread_mutex_destroy(&mutexBuffer);
    pthread_cond_destroy(&condBuffer);
}

void FFBufferQueue::release() {

    noticeThread();
    clearBuffer();

}

int FFBufferQueue::putBuffer(SAMPLETYPE *buffer, int size) {
    pthread_mutex_lock(&mutexBuffer);
    PcmBean *pcmBean = new PcmBean(buffer, size);
    LOGI("pcmsize %d",size);
    queueBuffer.push_back(pcmBean);
    pthread_cond_signal(&condBuffer);
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int FFBufferQueue::getBuffer(PcmBean **pcmBean) {

    pthread_mutex_lock(&mutexBuffer);

    while(playStatus!=-1)
    {
        if(queueBuffer.size() > 0)
        {
            *pcmBean = queueBuffer.front();
            queueBuffer.pop_front();
            break;
        } else{
            if(playStatus!=-1)
            {
                pthread_cond_wait(&condBuffer, &mutexBuffer);
            }
        }
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int FFBufferQueue::clearBuffer() {

    pthread_cond_signal(&condBuffer);
    pthread_mutex_lock(&mutexBuffer);
    while (!queueBuffer.empty())
    {
        PcmBean *pcmBean = queueBuffer.front();
        queueBuffer.pop_front();
        delete(pcmBean);
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int FFBufferQueue::getBufferSize() {
    int size = 0;
    pthread_mutex_lock(&mutexBuffer);
    size = queueBuffer.size();
    pthread_mutex_unlock(&mutexBuffer);
    return size;
}


int FFBufferQueue::noticeThread() {
    pthread_cond_signal(&condBuffer);
    return 0;
}

