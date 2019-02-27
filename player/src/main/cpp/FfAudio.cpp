//
// Created by Administrator on 2019/2/18.
//

#include "FfAudio.h"






FfAudio::FfAudio() {
    this->ffQueue = new FfQueue();
    this->ffPlayStatus = STATUS_PLAYING;
    this->outBuffer = (uint8_t*)av_malloc(BITRATE);
}

FfAudio::~FfAudio() {
    if(this->outBuffer!=NULL){
        free(this->outBuffer);
        this->outBuffer = NULL;
    }
}

void *openSLESCallBack(void *data) {
    FfAudio *audio = (FfAudio *) data;
    audio->createOpenSLES();
    pthread_exit(&audio->pthreadPlay);
}

void FfAudio::play() {
    pthread_create(&pthreadPlay, NULL, openSLESCallBack, this);
}


//读取pcm数据放入缓冲队列中，提供播放器进行读取
void simpleBufferCallBack(SLAndroidSimpleBufferQueueItf caller, void *pContext){
    FfAudio *audio = (FfAudio*)pContext;
    if(audio!=NULL){
        int dataSize = audio->resample();
        if(dataSize>0){
            (*audio->slAndroidSimpleBufferQueueItf)->Enqueue(audio->slAndroidSimpleBufferQueueItf,audio->outBuffer,dataSize);
        }
    }

}

void FfAudio::createOpenSLES() {
    //创建引擎对象
    slCreateEngine(&slengineObject,0,0,0,0,0);
    //初始化引擎
    (*slengineObject)->Realize(slengineObject,SL_BOOLEAN_FALSE);
    //获取引擎接口
    (*slengineObject)->GetInterface(slengineObject,SL_IID_ENGINE,&slEngineItf);

    //通过引擎接口创建混音器对象
    const SLInterfaceID slids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean slbs[1] = {SL_BOOLEAN_FALSE};
    slResult = (*slEngineItf)->CreateOutputMix(slEngineItf, &slOutputMixObject,1, slids, slbs);
    (void)slResult;
    //混音器对象初始化
    slResult = (*slOutputMixObject)->Realize(slOutputMixObject,SL_BOOLEAN_FALSE);
    (void)slResult;
    //混音器获取混音环境接口
    slResult = (*slOutputMixObject)->GetInterface(slOutputMixObject, SL_IID_ENVIRONMENTALREVERB, &slEnvironmentalReverbItf);
    if(SL_RESULT_SUCCESS==slResult){
        //通过混音环境接口设置混音环境
        slResult = (*slEnvironmentalReverbItf)->SetEnvironmentalReverbProperties(slEnvironmentalReverbItf,&slEnvironmentalReverbSettings);
        (void)slResult;
    }
    //混音器关联数据定位器
    SLDataLocator_OutputMix outputMix= {SL_DATALOCATOR_OUTPUTMIX,slOutputMixObject};

    //数据队列
    SLDataLocator_AndroidSimpleBufferQueue slDataLocator_androidSimpleBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    //数据格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//pcm格式
            2,//单声道
            SL_SAMPLINGRATE_48,//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,//16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数相同
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,//前左前右声道
            SL_BYTEORDER_LITTLEENDIAN,//结束符号
    };

    //数据输入源配置
    SLDataSource slDataSource = {&slDataLocator_androidSimpleBufferQueue,&pcm};
    //数据输出源配置
    SLDataSink slDataSink = {&outputMix,NULL};

    //创建播放器
    const SLInterfaceID slInterfaceID[3] = {SL_IID_BUFFERQUEUE,SL_IID_EFFECTSEND,SL_IID_VOLUME};
    const SLboolean  sLboolean[3] = {SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE,SL_BOOLEAN_FALSE};
    (*slEngineItf)->CreateAudioPlayer(slEngineItf,&slPCMPlayerObject,&slDataSource,&slDataSink,3,slInterfaceID,sLboolean);
    //初始化播放器
    (*slPCMPlayerObject)->Realize(slPCMPlayerObject,SL_BOOLEAN_FALSE);
    //获取播放接口
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject,SL_IID_PLAY,&slPlayItf);
    //获取缓冲队列
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject,SL_IID_BUFFERQUEUE,&slAndroidSimpleBufferQueueItf);
    //注册缓冲回调
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,simpleBufferCallBack,this);
    //获取音量接口
    (*slPCMPlayerObject)->GetInterface(slPCMPlayerObject,SL_IID_VOLUME,&slVolumeItf);
    //设置播放状态
    (*slPlayItf)->SetPlayState(slPlayItf,SL_PLAYSTATE_PLAYING);
    //调用缓冲回调开始播放
    simpleBufferCallBack(slAndroidSimpleBufferQueueItf,this);
}


int FfAudio::resample() {
    while (this->ffPlayStatus == STATUS_PLAYING) {
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
                        this->avFrame->channel_layout = av_get_default_channel_layout(this->avFrame->channels);
                        //如果没有声道数，有设置声道布局，通过声道布局设置声道数
                    } else if (this->avFrame->channels == 0 && this->avFrame->channel_layout > 0) {
                        this->avFrame->channels = av_get_channel_layout_nb_channels(this->avFrame->channels);
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
                        continue;
                    }

                    //开始转码,返回输出的采样率
                    int resample = swr_convert(
                            swrContext,
                            &this->outBuffer,//输出大小
                            this->avFrame->nb_samples,//输出采样个数
                            (const uint8_t**)this->avFrame->data,//原始数据
                            this->avFrame->nb_samples//输入采样个数
                    );
                    int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
                    this->dataSize = resample*outChannels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
                    if(LOGDEBUG){
                        LOGI("data size is %d",this->dataSize);
                    }
                    av_packet_free(&this->avPacket);
                    av_free(avPacket);
                    this->avPacket = NULL;
                    av_frame_free(&this->avFrame);
                    av_free(this->avFrame);
                    this->avFrame = NULL;
                    swr_free(&swrContext);
                    break;
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

    }
    return this->dataSize;
}



