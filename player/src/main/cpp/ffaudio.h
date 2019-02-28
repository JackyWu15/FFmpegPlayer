//
//用于保存音频属性和数据
//

#ifndef FFMPEGPLAYER_FFAudio_H
#define FFMPEGPLAYER_FFAudio_H

#include "ffqueue.h"
#include "ffcallback.h"
#define STATUS_LOADING 0
#define STATUS_PLAYING 1
#define STATUS_STOP 2;
#define BITRATE 48000*2*2

extern "C"{
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
};

class FFAudio {
public:
    //解码器上下文
    AVCodecContext* avCodecContext = NULL;
    //音频流角标
    int streamIndex;
    //音频流属性
    AVCodecParameters* codecpar = NULL;
    //播放状态
    int ffPlayStatus;
    //播放回调
    FFCallBack* ffCallBack = NULL;
    //数据队列
    FFQueue* ffQueue = NULL;
    //播放线程
    pthread_t pthreadPlay;
    //存放AVPacket
    AVPacket* avPacket = NULL;
    //解码出的音频帧
    AVFrame* avFrame = NULL;
    //重采样输出数据
    uint8_t *outBuffer = NULL;
    //数据大小
    int dataSize;
    SLresult slResult ;
    //引擎类和接口
    SLObjectItf slengineObject = NULL;
    SLEngineItf slEngineItf = NULL;
    //混音器
    SLObjectItf slOutputMixObject = NULL;
    //混音环境和参数
    SLEnvironmentalReverbSettings slEnvironmentalReverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLEnvironmentalReverbItf slEnvironmentalReverbItf= NULL;
    //播放器
    SLObjectItf slPCMPlayerObject = NULL;
    SLPlayItf slPlayItf = NULL;
    //音量接口
    SLVolumeItf slVolumeItf = NULL;
    //输入播放器队列
    SLAndroidSimpleBufferQueueItf  slAndroidSimpleBufferQueueItf =NULL;
public:
    FFAudio(FFCallBack* ffCallBack);
    ~FFAudio();

    void start();
    void createOpenSLES();
    int resample();
    void pause();
    void play();



};


#endif //FFMPEGPLAYER_FFAudio_H
