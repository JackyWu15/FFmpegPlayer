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
public:
    FFCallBack(JavaVM* javaVM,JNIEnv* jniEnv,jobject jobj);
    ~FFCallBack();

public:
    void onPrepareCallBack(int type);
};


#endif //FFMPEGPLAYER_FFCALLBCAK_H
