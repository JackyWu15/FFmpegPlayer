//
// Created by Administrator on 2019/2/18.
//

#include "FfCallBcak.h"

FfCallBack::FfCallBack(JavaVM *javaVM, JNIEnv *jniEnv, jobject jobj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jobj = jobj;

    //获取java方法methodId
    jclass jcls = jniEnv->GetObjectClass(jobj);
    if(jcls==NULL&&LOGDEBUG){
        LOGE("get class error")
        return;
    }
    this->jmethod_prepare = jniEnv->GetMethodID(jcls,"onPrepareCallBack","()V");
}

void FfCallBack::onPrepareCallBack(int type) {
    if(type==CALL_MAIN){
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_prepare);
    } else if(type==CALL_CHILD){
        JNIEnv* jniEnv;
        if(this->javaVM->AttachCurrentThread(&jniEnv,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGW("attach jniEnv failed!");
            }
            return;
        }
        jniEnv->CallVoidMethod(this->jobj,this->jmethod_prepare);
        this->javaVM->DetachCurrentThread();

    }

}
