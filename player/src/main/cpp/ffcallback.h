//
// Created by Administrator on 2019/2/18.
//

#ifndef FFMPEGPLAYER_FFCALLBCAK_H
#define FFMPEGPLAYER_FFCALLBCAK_H

#include "jni.h"
#include "androidlog.h"

#define CALL_MAIN (0)
#define CALL_CHILD (1)

class FFCallBack {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jobject jobj;

    jmethodID jmethod_prepare;
    jmethodID jmethod_load;
    jmethodID jmethod_progress;
    jmethodID jmethod_pause;
public:
    FFCallBack(JavaVM* javaVM,JNIEnv* jniEnv,jobject jobj);
    ~FFCallBack();

public:
    void onPrepareCallBack(int type);
    void onLoadCallBack(int type,bool status);
    void onPauseCallBack(int type,bool status);
    void onProgressCallBack(int type,int currentTime,int totalTime);
};


#endif //FFMPEGPLAYER_FFCALLBCAK_H
