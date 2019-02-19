//
// Created by Administrator on 2019/2/18.
//

#ifndef FFMPEGPLAYER_FFAUDIO_H
#define FFMPEGPLAYER_FFAUDIO_H
extern "C"{
#include "libavformat/avformat.h"
};

class FfAudio {
public:
    AVCodecContext* avCodecContext = NULL;
    int streamIndex;
    AVCodecParameters* codecpar = NULL;
public:
    FfAudio();
    ~FfAudio();
};


#endif //FFMPEGPLAYER_FFAUDIO_H
