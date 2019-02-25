//
//用于保存音频属性和数据
//

#ifndef FFMPEGPLAYER_FFAUDIO_H
#define FFMPEGPLAYER_FFAUDIO_H

#include "FfQueue.h"
extern "C"{
#include "libavformat/avformat.h"
};

class FfAudio {
public:
    //解码器上下文
    AVCodecContext* avCodecContext = NULL;
    //音频流角标
    int streamIndex;
    //音频流属性
    AVCodecParameters* codecpar = NULL;
    //播放状态
    int ffPlayStatus;
    //数据队列
    FfQueue* ffQueue = NULL;
public:
    FfAudio();
    ~FfAudio();
};


#endif //FFMPEGPLAYER_FFAUDIO_H
