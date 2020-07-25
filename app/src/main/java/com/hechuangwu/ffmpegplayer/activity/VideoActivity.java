package com.hechuangwu.ffmpegplayer.activity;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.UserHandle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.hechuangwu.ffmpegplayer.R;
import com.hechuangwu.player.ffplayer.FFPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

import java.io.File;

public class VideoActivity extends Activity implements SurfaceHolder.Callback {
    private static final String TAG = "VideoActivity";
    private SurfaceView mSv_video;
    private FFPlayer mFFPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.activity_video );
        mSv_video = findViewById( R.id.sv_video );
        SurfaceHolder holder = mSv_video.getHolder();
        holder.addCallback( this );
    }

    @Override
    public void surfaceCreated(final SurfaceHolder holder) {
        String path = getExternalCacheDir().getAbsolutePath()+ File.separator + "Forrest_Gump_IMAX.flv";
        Log.i(TAG,getExternalCacheDir().getAbsolutePath());
        mFFPlayer = new FFPlayer();
        mFFPlayer.setFilePath( path );
        mFFPlayer.videoPrepare();
        mFFPlayer.setOnPlayerListener( new OnPlayerListener() {
            @Override
            public void OnLoad(boolean type) {

            }

            @Override
            public void OnPrepare() {
                mFFPlayer.videoStart( holder.getSurface() );
            }

            @Override
            public void OnPause(boolean type) {

            }

            @Override
            public void onError(int type, String msg) {

            }

            @Override
            public void onProgress(int currentTime, int totalTime) {

            }

            @Override
            public void onComplete() {

            }

            @Override
            public void onPCMDB(int db) {

            }

            @Override
            public void onRecordTime(int recordTime) {

            }
        } );
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

}
