//
// Created by Administrator on 2019/2/15.
//

#ifndef FFMPEGPLAYER_ANDROIDLOG_H
#define FFMPEGPLAYER_ANDROIDLOG_H
#include "android/log.h"
#define LOGDEBUG true

#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"hechuangwu",FORMAT,##__VA_ARGS__);
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"hechuangwu",FORMAT,##__VA_ARGS__);
#define LOGW(FORMAT,...) __android_log_print(ANDROID_LOG_WARN,"hechuangwu",FORMAT,##__VA_ARGS__);

#endif //FFMPEGPLAYER_ANDROIDLOG_H
