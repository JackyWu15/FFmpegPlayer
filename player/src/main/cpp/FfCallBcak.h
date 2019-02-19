//
// Created by Administrator on 2019/2/18.
//

#ifndef FFMPEGPLAYER_FFCALLBCAK_H
#define FFMPEGPLAYER_FFCALLBCAK_H

#include "jni.h"
#include "AndroidLog.h"

#define CALL_MAIN (0)
#define CALL_CHILD (1)

class FfCallBack {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;
    jobject jobj;

    jmethodID jmethod_prepare;
public:
    FfCallBack(JavaVM* javaVM,JNIEnv* jniEnv,jobject jobj);
    ~FfCallBack();

public:
    void onPrepareCallBack(int type);
};


#endif //FFMPEGPLAYER_FFCALLBCAK_H
