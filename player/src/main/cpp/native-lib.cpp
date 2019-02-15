#include <jni.h>
#include <string>
using namespace std;



extern "C"
JNIEXPORT jstring JNICALL
Java_com_hechuangwu_player_ffplayer_FfPlayer_stringFromJNI(JNIEnv *env, jobject instance) {

    string hello = "hello from c++ ";

    return env->NewStringUTF(hello.c_str());
}