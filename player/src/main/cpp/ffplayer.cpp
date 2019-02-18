#include <jni.h>
#include <string>
using namespace std;


extern "C"
JNIEXPORT void JNICALL
Java_com_hechuangwu_player_ffplayer_FfPlayer__1prepare(JNIEnv *env, jobject instance,
                                                       jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);

    // TODO

    env->ReleaseStringUTFChars(filePath_, filePath);
}