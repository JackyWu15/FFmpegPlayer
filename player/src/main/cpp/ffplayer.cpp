#include <string>
#include "ffmpeg.h"

using namespace std;

JavaVM *javaVM;
FFCallBack *ffCallBack;
FFmpeg *ffmpeg;

/**
 * 获取JavaVM
 * 必须返回版本号
 */
extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    JNIEnv *jniEnv;
    if (vm->GetEnv((void **) (&jniEnv), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1prepare(JNIEnv *env, jobject instance,
                                                       jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);
    if (ffmpeg == NULL) {
        if (ffCallBack == NULL) {
            //对象不能跨线程使用，需生成全局变量
            ffCallBack = new FFCallBack(javaVM, env, env->NewGlobalRef(instance));
        }

        ffmpeg = new FFmpeg(ffCallBack, filePath);
        ffmpeg->prepare();
    }


    env->ReleaseStringUTFChars(filePath_, filePath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1start(JNIEnv *env, jobject instance) {
    if(ffmpeg!=NULL){
        ffmpeg->start();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1pause(JNIEnv *env, jobject instance) {

    if(ffmpeg!=NULL){
        ffmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1play(JNIEnv *env, jobject instance) {
    if(ffmpeg!=NULL){
        ffmpeg->play();
    }

}