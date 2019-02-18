package com.hechuangwu.player.log;

import android.util.Log;

/**
 * Created by cwh on 2019/2/15.
 * 功能:
 */
public class MyLog {
    public static void d(String msg){
        Log.i( "ffmpeg4debug", msg );
    }
    public static void e(String msg){
        Log.i( "ffmpeg4error", msg );
    }
}
