#include <string>
#include <iostream>
#include "ffmpeg.h"

using namespace std;

JavaVM *javaVM;
FFCallBack *ffCallBack;
FFmpeg *ffmpeg;
pthread_t playerStart;
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

void *startCallBack(void *data) {
    FFmpeg *ffmpeg = (FFmpeg *) data;
    ffmpeg->start();
    pthread_exit(&playerStart);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1start(JNIEnv *env, jobject instance) {
    if (ffmpeg != NULL) {
        pthread_create(&playerStart, NULL, startCallBack, ffmpeg);
//        ffmpeg->start();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1pause(JNIEnv *env, jobject instance) {
    if (ffmpeg != NULL) {
        ffmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1play(JNIEnv *env, jobject instance) {
    if (ffmpeg != NULL) {
        ffmpeg->play();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1stop(JNIEnv *env, jobject instance) {

    if (ffmpeg != NULL) {
        ffmpeg->release();
        delete (ffmpeg);
        ffmpeg = NULL;
    }
    if (ffCallBack != NULL) {
        delete (ffCallBack);
        ffCallBack = NULL;
    }


}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1seek(JNIEnv *env, jobject instance, jint seconds) {

    if (ffmpeg != NULL) {
        ffmpeg->seek(seconds);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1setVolume(JNIEnv *env, jobject instance,
                                                         jint percent) {

    if (ffmpeg != NULL) {
        ffmpeg->setVolume(percent);
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1setChannel(JNIEnv *env, jobject instance,
                                                          jint channel) {
    if (ffmpeg != NULL) {
        ffmpeg->setChannel(channel);
    }
}



extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1setPitch(JNIEnv *env, jobject instance,
                                                        jfloat pitch) {
    if (ffmpeg != NULL) {
        ffmpeg->setPitch(pitch);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1setTempo(JNIEnv *env, jobject instance,
                                                        jfloat tempo) {

    if (ffmpeg != NULL) {
        ffmpeg->setTempo(tempo);
    }

}




extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1video_1prepare(JNIEnv *env, jobject instance,
                                                              jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);
    if (ffmpeg == NULL) {
        if (ffCallBack == NULL) {
            ffCallBack = new FFCallBack(javaVM, env, env->NewGlobalRef(instance));
        }
        ffmpeg = new FFmpeg(ffCallBack, filePath);
        ffmpeg->video_prepare();
    }

    env->ReleaseStringUTFChars(filePath_, filePath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FFPlayer__1video_1start(JNIEnv *env, jobject instance,
                                                            jobject surface) {
    if (ffmpeg != NULL) {
        ffmpeg->video_start(env, surface);
    }
}
