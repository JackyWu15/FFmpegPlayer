package com.hechuangwu.player.listener;

/**
 * Created by cwh on 2019/2/15.
 * 功能:
 */
public interface OnPlayerListener {
    void OnLoad(boolean type);
    void OnPrepare();
    void OnPause(boolean type);
    void onError(int type,String msg);
    void onProgress(int currentTime,int totalTime);
    void onComplete( );
    void onPCMDB(int db);
}
