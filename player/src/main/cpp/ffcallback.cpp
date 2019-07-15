//
// Created by Administrator on 2019/2/18.
//

#include "ffcallback.h"

FFCallBack::FFCallBack(JavaVM *javaVM, JNIEnv *jniEnv, jobject jobj) {
    this->javaVM = javaVM;
    this->jniEnv = jniEnv;
    this->jobj = jobj;

    //获取java方法methodId
    jclass jcls = jniEnv->GetObjectClass(jobj);
    if(jcls== nullptr&&LOGDEBUG){
        LOGE("get class error")
        return;
    }
    this->jmethod_prepare = jniEnv->GetMethodID(jcls,"onPrepareCallBack","()V");
    this->jmethod_load = jniEnv->GetMethodID(jcls,"onLoadCallBack","(Z)V");
    this->jmethod_progress = jniEnv->GetMethodID(jcls,"onProgressCallBack","(II)V");
    this->jmethod_pause = jniEnv->GetMethodID(jcls,"onPauseCallBack","(Z)V");
    this->jmethod_error = jniEnv->GetMethodID(jcls,"onErrorCallBack","(ILjava/lang/String;)V");
    this->jmethod_complete = jniEnv->GetMethodID(jcls,"onCompleteCallBack","()V");
    this->jmethod_db = jniEnv->GetMethodID(jcls,"onPCMDBCallBack","(I)V");

}
FFCallBack::~FFCallBack() {

}

void FFCallBack::onPrepareCallBack(int type) {
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

void FFCallBack::onLoadCallBack(int type,bool load) {
    if(type==CALL_MAIN){
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_load,load);
    } else if(type==CALL_CHILD){
        JNIEnv* env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGW("attach jniEnv failed!");
                return;
            }
        }
        env->CallVoidMethod(this->jobj,this->jmethod_load,load);
        this->javaVM->DetachCurrentThread();
    }
}
void FFCallBack::onPauseCallBack(int type, bool status) {
    if(type==CALL_MAIN){
//        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_pause,status);
    } else if(type==CALL_CHILD){
        JNIEnv* env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGW("attach jniEnv failed!");
            }
            return;
        }
        env->CallVoidMethod(this->jobj,this->jmethod_pause,status);
    }
}

void FFCallBack::onProgressCallBack(int type, int currentTime, int totalTime) {
    if(type==CALL_MAIN){
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_progress,currentTime,totalTime);
    } else if(type==CALL_CHILD){
        JNIEnv* env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGE("attach jniEnv failed!");
            }
            return;
        }
        env->CallVoidMethod(this->jobj,this->jmethod_progress,currentTime,totalTime);
        this->javaVM->DetachCurrentThread();
    }

}

void FFCallBack::onErrorCallBack(int type,int code,char* msg){
    if(type==CALL_MAIN){
        jstring jmsg = this->jniEnv->NewStringUTF(msg);
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_error,code,jmsg);
        this->jniEnv->DeleteLocalRef(jmsg);
    } else if(type==CALL_CHILD){
        JNIEnv*env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGE("attach jniEnv failed!");
            }
            return;
        }
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(this->jobj,this->jmethod_error,code,jmsg);
        this->javaVM->DetachCurrentThread();
    }
}

void FFCallBack::onCompleteCallBack(int type) {
    if(type==CALL_MAIN){
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_complete);
    } else if(type==CALL_CHILD){
        JNIEnv* env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGE("attach jniEnv failed!");
            }
            return;
        }
        env->CallVoidMethod(this->jobj,this->jmethod_complete);
        this->javaVM->DetachCurrentThread();
    }
}

void FFCallBack::onPCMDBCallBack(int type,int db) {
    if(type==CALL_MAIN){
        this->jniEnv->CallVoidMethod(this->jobj,this->jmethod_db,db);
    } else if(type==CALL_CHILD){
        JNIEnv* env;
        if(this->javaVM->AttachCurrentThread(&env,0)!=JNI_OK){
            if(LOGDEBUG){
                LOGE("attach jniEnv failed!");
            }
            return;
        }
        env->CallVoidMethod(this->jobj,this->jmethod_db,db);
        this->javaVM->DetachCurrentThread();
    }
}


