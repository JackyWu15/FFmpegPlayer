//
// Created by Administrator on 2019/2/18.
//

#include "ffaudio.h"


FFAudio::FFAudio(FFCallBack *ffCallBack) {
    this->ffCallBack = ffCallBack;
    this->ffQueue = new FFQueue();
    this->ffPlayStatus = STATUS_INIT;
    this->outBuffer = (uint8_t *) av_malloc(BITRATE);

    this->sampleBuffer = static_cast<SAMPLETYPE *>(malloc(BITRATE));
    this->soundTouch = new SoundTouch();
    this->soundTouch->setSampleRate(SAMPLERATE);
    this->soundTouch->setChannels(2);
}

FFAudio::~FFAudio() {

}

void *openSLESCallBack(void *data) {
    FFAudio *audio = (FFAudio *) data;
    audio->createOpenSLES();
    pthread_exit(&audio->pthreadPlay);
}

void FFAudio::start() {
    pthread_create(&pthreadPlay, NULL, openSLESCallBack, this);
}




/**
 * 从队列中取出帧数据解析成pcm数据
 * @param out_buffer pcm数据
 * @return
 */
int FFAudio::resample(void **out_buffer) {
    if (this->ffQueue->getQueueSize() == 0) {
        if (this->ffPlayStatus != STATUS_LOADING) {
            this->ffPlayStatus = STATUS_LOADING;
        }
        this->ffCallBack->onLoadCallBack(CALL_CHILD, true);
        return -1;
    } else {
        if (this->ffPlayStatus != STATUS_PLAYING) {
            this->ffPlayStatus = STATUS_PLAYING;
        }
        this->ffCallBack->onLoadCallBack(CALL_CHILD, false);
    }
    this->avPacket = av_packet_alloc();
    if (this->ffQueue->popAVPacket(this->avPacket) == 0) {
        //AVPacket传入发送进行解码
        int ret = avcodec_send_packet(this->avCodecContext, this->avPacket);
        if (ret == 0) {
            this->avFrame = av_frame_alloc();
            //如果AVPacket解析成功，接收解析后的AVFrame
            int ret = avcodec_receive_frame(this->avCodecContext, this->avFrame);
            if (ret == 0) {
                //如果有声道数，没有设置声道布局，通过声道数设置声道布局
                if (this->avFrame->channels > 0 && this->avFrame->channel_layout == 0) {
                    this->avFrame->channel_layout = av_get_default_channel_layout(
                            this->avFrame->channels);
                    //如果没有声道数，有设置声道布局，通过声道布局设置声道数
                } else if (this->avFrame->channels == 0 && this->avFrame->channel_layout > 0) {
                    this->avFrame->channels = av_get_channel_layout_nb_channels(
                            this->avFrame->channels);
                }

                //对AVFrame进行重采样，这里保持和源数据相同参数
                SwrContext *swrContext;
                swrContext = swr_alloc_set_opts(
                        NULL, //如果为NULL则创建一个新的SwrContext，否则对已有的SwrContext进行参数设置
                        AV_CH_LAYOUT_STEREO,//传出声道布局
                        AV_SAMPLE_FMT_S16,//传出采样位数
                        this->avFrame->sample_rate,//传出采样率
                        this->avFrame->channel_layout,//传入声道布局
                        (AVSampleFormat) this->avFrame->format,//传入采样位数
                        this->avFrame->sample_rate,//传入采样率
                        NULL, NULL//日志参数
                );

                //初始化失败
                if (!swrContext || swr_init(swrContext) < 0) {
                    av_packet_free(&this->avPacket);
                    av_free(avPacket);
                    this->avPacket = NULL;
                    av_frame_free(&this->avFrame);
                    av_free(this->avFrame);
                    this->avFrame = NULL;
                    swr_free(&swrContext);
                    return -1;
                }

                //开始转码,返回输出的采样率
                nb = swr_convert(
                        swrContext,
                        &this->outBuffer,//输出大小
                        this->avFrame->nb_samples,//输出采样个数
                        (const uint8_t **) this->avFrame->data,//原始数据
                        this->avFrame->nb_samples//输入采样个数
                );
                int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
                this->dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
                if (LOGDEBUG) {
                    LOGI("data size is %d,nb size is %d", this->dataSize,nb);
                }
                this->frameDuration = avFrame->pts * av_q2d(this->avRational);
                if (this->frameDuration > this->currentTime) {
                    this->currentTime = this->frameDuration;
                } else {
                    this->frameDuration = this->currentTime;
                }

                *out_buffer = outBuffer;
                av_packet_free(&this->avPacket);
                av_free(avPacket);
                this->avPacket = NULL;
                av_frame_free(&this->avFrame);
                av_free(this->avFrame);
                this->avFrame = NULL;
                swr_free(&swrContext);
            } else {
                av_packet_free(&this->avPacket);
                av_free(avPacket);
                this->avPacket = NULL;
                av_frame_free(&this->avFrame);
                av_free(this->avFrame);
                this->avFrame = NULL;
            }
        } else {
            av_packet_free(&this->avPacket);
            av_free(avPacket);
            avPacket = NULL;
        }
    } else {
        av_packet_free(&this->avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }

    return this->dataSize;
}

/**
 *  soundtouch处理pcm数据，处理后存入sampleBuffer中,送入opengsl es播放
 *  函数逻辑是：解析一帧返回数据，然后处理多少个，便先输入多少到opensl es中，再回到279行重新取，直到取出为0，说明一帧数据处理完毕，紧接270行设置为true，250继续取下一帧数据
 * @return 返回处理采样个数
 */
int FFAudio::getSoundTouchData() {
    while (this->ffPlayStatus != STATUS_STOP){
        out_buffer = NULL;
        if(isFinish){
            isFinish = false;
            dataSize = resample(reinterpret_cast<void **>(&out_buffer));
            LOGI("resample size is %d",dataSize);
            if(dataSize>0){
                //soundtouch只支持16位，即2个字节的数组，而ffmpeg数据是1个字节为存储的数组，需做转换
                for(int i=0;i<dataSize/2+1;i++){
                    sampleBuffer[i] = (out_buffer[i * 2] | ((out_buffer[i * 2 + 1]) << 8));
                }
                //nb为采样率，这里理论上等于dataSize/4
                this->soundTouch->putSamples(sampleBuffer,nb);
                //返回实际处理的采样个数，个数为0说明取出完成,datasize/4指定1个采样1字节
                num = this->soundTouch->receiveSamples(sampleBuffer, dataSize / 4);
            } else{
                this->soundTouch->flush();
            }

        }

        LOGI("receive num is %d",num);
        if(num==0){
            isFinish = true;
            continue;
        } else{
            if(out_buffer==NULL){
                num = this->soundTouch->receiveSamples(sampleBuffer,dataSize/4);
                if(num==0){
                    isFinish = true;
                    continue;
                }
            }
            return num;
        }
    }
    return 0;
}


//读取pcm数据放入缓冲队列中，提供播放器进行读取
void simpleBufferCallBack(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    FFAudio *audio = (FFAudio *) pContext;
    if (audio != NULL) {
        int dataSize = audio->getSoundTouchData();
        audio->currentTime += dataSize / BITRATE;
        LOGI("current time %d,new time %d", audio->currentTime, audio->newTime);
        if (audio->currentTime - audio->newTime > 0.1) {
            audio->newTime = audio->currentTime;
            audio->ffCallBack->onProgressCallBack(CALL_CHILD, audio->currentTime, audio->allDuration);
        }
        if (dataSize > 0) {
            int db = audio->getPCMDB(reinterpret_cast<char *>(audio->sampleBuffer), dataSize * 4);
            audio->ffCallBack->onPCMDBCallBack(CALL_CHILD,db);
//            (*audio->slAndroidSimpleBufferQueueItf)->Enqueue(audio->slAndroidSimpleBufferQueueItf,audio->outBuffer, dataSize);
            (*audio->slAndroidSimpleBufferQueueItf)->Enqueue(audio->slAndroidSimpleBufferQueueItf,audio->sampleBuffer, dataSize*4);//以4个字节播放，因为双声道，16位
        }
    }

}

void FFAudio::createOpenSLES() {
    //创建引擎对象
    slCreateEngine(&slEngineObject, 0, 0, 0, 0, 0);
    //初始化引擎
    (*slEngineObject)->Realize(slEngineObject, SL_BOOLEAN_FALSE);
    //获取引擎接口
    (*slEngineObject)->GetInterface(slEngineObject, SL_IID_ENGINE, &slEngineItf);

    //通过引擎接口创建混音器对象
    const SLInterfaceID slids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean slbs[1] = {SL_BOOLEAN_FALSE};
    slResult = (*slEngineItf)->CreateOutputMix(slEngineItf, &slOutputMixObject, 1, slids, slbs);
    (void) slResult;
    //混音器对象初始化
    slResult = (*slOutputMixObject)->Realize(slOutputMixObject, SL_BOOLEAN_FALSE);
    (void) slResult;
    //混音器获取混音环境接口
    slResult = (*slOutputMixObject)->GetInterface(slOutputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                                  &slEnvironmentalReverbItf);
    if (SL_RESULT_SUCCESS == slResult) {
        //通过混音环境接口设置混音环境
        slResult = (*slEnvironmentalReverbItf)->SetEnvironmentalReverbProperties(
                slEnvironmentalReverbItf, &slEnvironmentalReverbSettings);
        (void) slResult;
    }
    //混音器关联数据定位器
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, slOutputMixObject};

    //数据队列
    SLDataLocator_AndroidSimpleBufferQueue slDataLocator_androidSimpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //数据格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//pcm格式
            2,//单声道
            SL_SAMPLINGRATE_48,//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,//16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数相同
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//前左前右声道
            SL_BYTEORDER_LITTLEENDIAN,//结束符号
    };

    //数据输入源配置
    SLDataSource slDataSource = {&slDataLocator_androidSimpleBufferQueue, &pcm};
    //数据输出源配置
    SLDataSink slDataSink = {&outputMix, NULL};

    //创建播放器
    const SLInterfaceID slInterfaceID[4] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME,SL_IID_MUTESOLO};
    const SLboolean sLboolean[4] = {SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE};
    (*slEngineItf)->CreateAudioPlayer(slEngineItf, &slPCMPlayerObject, &slDataSource, &slDataSink,
                                      4, slInterfaceID, sLboolean);
    //初始化播放器
    (*slPCMPlayerObject)->Realize(slPCMPlayerObject, SL_BOOLEAN_FALSE);
    //获取播放接口
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject, SL_IID_PLAY, &slPlayItf);
    //获取缓冲队列
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject, SL_IID_BUFFERQUEUE,
                                       &slAndroidSimpleBufferQueueItf);
    //注册缓冲回调
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,
                                                       simpleBufferCallBack, this);
    //获取音量接口
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject, SL_IID_VOLUME, &slVolumeItf);

    //获取声道接口
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject, SL_IID_MUTESOLO, &slMuteSoloItf);
    //设置播放状态
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);

    //调用缓冲回调开始播放
    simpleBufferCallBack(slAndroidSimpleBufferQueueItf, this);
}


void FFAudio::pause() {
    if (this->slPlayItf != NULL) {
        (*this->slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PAUSED);
        this->ffCallBack->onPauseCallBack(CALL_MAIN, true);
    }
}

void FFAudio::play() {
    if (this->slPlayItf != NULL) {
        (*this->slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
        this->ffCallBack->onPauseCallBack(CALL_MAIN, false);
    }
}

void FFAudio::release() {
    if (ffQueue != NULL) {
        delete (ffQueue);
        ffQueue = NULL;
    }

    if (slPCMPlayerObject != NULL) {
        (*slPCMPlayerObject)->Destroy(slPCMPlayerObject);
        slPCMPlayerObject = NULL;
        slPlayItf = NULL;
        slAndroidSimpleBufferQueueItf = NULL;
        slVolumeItf = NULL;
    }

    if (slOutputMixObject != NULL) {
        (*slOutputMixObject)->Destroy(slOutputMixObject);
        slOutputMixObject = NULL;
        slEnvironmentalReverbItf = NULL;
    }

    if (slEngineObject != NULL) {
        (*slEngineObject)->Destroy(slEngineObject);
        slEngineObject = NULL;
        slEngineItf = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (outBuffer != NULL) {
        free(outBuffer);
        outBuffer = NULL;
    }

}


void FFAudio::setPitch(float pitch) {
    if(this->soundTouch!=NULL){
        this->soundTouch->setPitch(pitch);
    }
}

void FFAudio::setTempo(float tempo) {
    if(this->soundTouch!=NULL){
        this->soundTouch->setTempo(tempo);
    }
}



void FFAudio::setVolume(int percent) {
    if (slVolumeItf != NULL) {
        if (percent > 30) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -20);
        } else if (percent > 25) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -22);
        } else if (percent > 20) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -25);
        } else if (percent > 15) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -28);
        } else if (percent > 10) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -30);
        } else if (percent > 5) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -34);
        } else if (percent > 3) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -37);
        } else if (percent > 0) {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -40);
        } else {
            (*slVolumeItf)->SetVolumeLevel(slVolumeItf, (100 - percent) * -100);
        }
    }
}

void FFAudio::setChannel(int channel) {
    if(slMuteSoloItf!=NULL) {
        if (channel == 0)//right
        {
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 1, false);
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 0, true);
        } else if (channel == 1)//left
        {
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 1, true);
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 0, false);
        } else if (channel == 2)//center
        {
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 1, false);
            (*slMuteSoloItf)->SetChannelMute(slMuteSoloItf, 0, false);
        }
    }
}

/**
 * 获取声音分贝值
 * @param pcmData
 * @param pcmSize
 * @return
 */
int FFAudio::getPCMDB(char *pcmData, size_t pcmSize) {
    int db = 0;
    short int perValue = 0;
    double sum = 0;
    for(int i =0;i<pcmSize;i+=2){
        memcpy(&perValue,pcmData+i,2);
        sum += abs(perValue);
    }
    sum = sum / (pcmSize / 2);
    if(sum > 0)
    {
        db = (int)20.0 *log10(sum);
    }
    return db;
}








