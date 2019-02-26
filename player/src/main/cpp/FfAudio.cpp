//
// Created by Administrator on 2019/2/18.
//

#include "FfAudio.h"

FfAudio::FfAudio() {
    this->ffQueue = new FfQueue();
    this->ffPlayStatus = STATUS_PLAYING;
    this->outBuffer = (uint8_t*)av_malloc(44100*2*2);
}

FfAudio::~FfAudio() {
    if(this->outBuffer!=NULL){
        free(this->outBuffer);
        this->outBuffer = NULL;
    }
}

void *resampleCallBack(void *data) {
    FfAudio *audio = (FfAudio *) data;
    audio->resample();
    pthread_exit(&audio->pthreadPlay);
}

void FfAudio::play() {
    pthread_create(&pthreadPlay, NULL, resampleCallBack, this);
}

FILE* outFile = fopen("/storage/emulated/0/myout.pcm","w");

void FfAudio::resample() {
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
                    fwrite(this->outBuffer,1,this->dataSize,outFile);

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

    }
}



