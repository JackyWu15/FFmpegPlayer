package com.hechuangwu.player.ffplayer;

import android.text.TextUtils;

import com.hechuangwu.player.listener.OnPlayerListener;
import com.hechuangwu.player.log.MyLog;

/**
 * Created by cwh on 2019/2/15.
 * 功能:
 */
public class FfPlayer {
    static {
        System.loadLibrary("ffplayer");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }
    private String filePath;
    private OnPlayerListener onPlayerListener;

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    private void setOnPlayerListener(OnPlayerListener onPlayerListener){
        this.onPlayerListener = onPlayerListener;
    }
    private void onPrepareCallBack(){
        if(this.onPlayerListener!=null){
            this.onPlayerListener.OnPrepare();
        }

    }

    public void prepare(){
        if(!TextUtils.isEmpty( filePath )){
            new Thread( new Runnable() {
                @Override
                public void run() {
                    _prepare(filePath);
                }
            } ).start();
        }else {
            MyLog.e( "filePath is empty!" );
        }
    }
    private native void _prepare(String filePath);
}
