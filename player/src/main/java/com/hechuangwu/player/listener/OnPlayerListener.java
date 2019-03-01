package com.hechuangwu.player.listener;

/**
 * Created by cwh on 2019/2/15.
 * 功能:
 */
public interface OnPlayerListener {
    void OnLoad(boolean type);
    void OnPrepare();
    void OnPause(boolean type);
    void onProgress(int currentTime,int totalTime);
}
