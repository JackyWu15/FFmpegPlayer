//
// Created by Administrator on 2019/2/18.
//

#include "Ffmpeg.h"

Ffmpeg::Ffmpeg(FfCallBack *ffCallBack, const char *filePath) {
    this->ffCallBack = ffCallBack;
    this->filePath = filePath;
}

void *decodeCallBack(void *data) {
    Ffmpeg *ffmpeg = (Ffmpeg *) data;
    ffmpeg->decodeAudio();
    pthread_exit(&ffmpeg->decodeThread);
}

void Ffmpeg::prepare() {
    pthread_create(&decodeThread, NULL, decodeCallBack, this);
}

void Ffmpeg::decodeAudio() {
    //注册所有解码器
    av_register_all();
    //初始化网络连接组件
    avformat_network_init();
    //分配上下文
    avFormatContext = avformat_alloc_context();
    //打开地址成功返回0
    if (avformat_open_input(&avFormatContext, filePath, NULL, NULL) != 0) {
        if (LOGDEBUG) {
            LOGW("open filePath is failed, filePath is %s", filePath);
            return;
        }
    }

    //找到流媒体成功返回大于等于0
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (LOGDEBUG) {
            LOGW("open stream is failed, filePath is %s", filePath)
            return;
        }
    }

    //从流媒体中找音频流并进行存储
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (ffAudio == NULL) {
                ffAudio = new FfAudio();
                ffAudio->streamIndex = i;
                ffAudio->codecpar = avFormatContext->streams[i]->codecpar;
            }
        }
    }
    //根据流属性id找到解码器
    AVCodec *pCodec = avcodec_find_decoder(ffAudio->codecpar->codec_id);
    if (!pCodec) {
        if (LOGDEBUG) {
            LOGW("can not find decoder!")
        }
        return;
    }
    //通过解码器创建解码器上下文
    this->ffAudio->avCodecContext = avcodec_alloc_context3(pCodec);

    //将解码器中的属性复制到解码器上下文中
    if (avcodec_parameters_to_context(this->ffAudio->avCodecContext, this->ffAudio->codecpar) < 0) {
        if (LOGDEBUG) {
            LOGW("can not fill the avCodec to avCodecContext!")
        }
        return;
    }

    //打开解码器
    if (avcodec_open2(this->ffAudio->avCodecContext, pCodec, 0) < 0) {
        if (LOGDEBUG) {
            LOGW("can not open avCodec!")
        }
        return;
    }

    //prepare成功，通知java层
    this->ffCallBack->onPrepareCallBack(CALL_CHILD);

}

//从流中解码出每一帧
void Ffmpeg::start() {
    if (this->ffAudio == NULL) {
        if (LOGDEBUG) {
            LOGE("audio is null!");
        }
        return;
    }
    //开启线程，解码AVPacket并重采样生成PCM数据
    this->ffAudio->play();

    //保存AVPacket到队列中
    int count = 0;
    while (this->ffAudio->ffPlayStatus == STATUS_PLAYING) {
        AVPacket *pPacket = av_packet_alloc();
        if (av_read_frame(this->avFormatContext, pPacket) == 0) {
            if (pPacket->stream_index == this->ffAudio->streamIndex) {
                count++;
                if (LOGDEBUG) {
                LOGI("解码到第 %d 帧", count);
                }
                this->ffAudio->ffQueue->setFfPlayStatus(this->ffAudio->ffPlayStatus);
                this->ffAudio->ffQueue->pushAVPacket(pPacket);
            } else{
                av_packet_free(&pPacket);
                av_free(pPacket);
                pPacket = NULL;
            }
        } else{
            av_packet_free(&pPacket);
            av_free(pPacket);
            pPacket = NULL;
            //出队完成退出循环
            while (this->ffAudio->ffPlayStatus==STATUS_PLAYING){
                if(this->ffAudio->ffQueue->getQueueSize()>0){
                    continue;
                } else{
                    this->ffAudio->ffPlayStatus = STATUS_STOP;
                }
            }
            break;
        }
    }

    if(LOGDEBUG){
        LOGI("decode all finished!")
    }

}
