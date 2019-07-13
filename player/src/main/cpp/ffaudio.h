//
//用于保存音频属性和数据
//

#ifndef FFMPEGPLAYER_FFAudio_H
#define FFMPEGPLAYER_FFAudio_H

#include "ffqueue.h"
#include "ffcallback.h"
#include "SoundTouch.h"
using namespace soundtouch;

#define STATUS_INIT (-1)
#define STATUS_LOADING (0)
#define STATUS_PLAYING (1)
#define STATUS_STOP (2)
#define STATUS_SEEK (3)

#define SAMPLERATE 48000
#define BITRATE SAMPLERATE*2*2

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
    AVCodecParameters* avCodecpar = NULL;
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
    SLObjectItf slEngineObject = NULL;
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
    //声道接口
    SLMuteSoloItf slMuteSoloItf = NULL;

    //输入播放器队列
    SLAndroidSimpleBufferQueueItf  slAndroidSimpleBufferQueueItf =NULL;
    //avframe时间单位
    AVRational avRational;
    //总时间(秒)
    int allDuration;
    //frame的当前时间
    int frameDuration;
    //当前时间
    int currentTime;
    //时间间隔
    int newTime;

    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    bool isFinish = true;
    uint8_t *out_buffer = NULL;
    int nb = 0;
    int num = 0;

public:
    FFAudio(FFCallBack* ffCallBack);
    ~FFAudio();

    void start();
    void createOpenSLES();
    int resample(void **buffer);
    void pause();
    void play();
    void release();
    void setVolume(int percent);
    void setChannel(int channel);

    int getSoundTouchData();
    void setPitch(float pitch);
    void setTempo(float tempo);
};


#endif //FFMPEGPLAYER_FFAudio_H
