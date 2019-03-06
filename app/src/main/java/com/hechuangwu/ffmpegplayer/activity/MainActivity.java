package com.hechuangwu.ffmpegplayer.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.hechuangwu.ffmpegplayer.R;
import com.hechuangwu.ffmpegplayer.utls.TimeUtils;
import com.hechuangwu.player.ffplayer.FFPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

import java.io.File;

public class MainActivity extends Activity implements View.OnClickListener {


    private final static int PERMISSION_CODE = 1;
    private FFPlayer mFFmpegPlayer;
    private Button mBtPlay;
    private String mFilePath;
    private Button mBtPause;
    private TextView mTvCurrentTime;
    private TextView mTvTotalTime;
    private Button mBtStop;
    private boolean isStop = true;
    private boolean isPause;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        initView();
        initData();
        initEvent();
    }
    private void initView() {
        setContentView( R.layout.activity_main );
        mBtPause = findViewById( R.id.bt_pause );
        mBtPlay = findViewById( R.id.bt_play );
        mBtStop = findViewById( R.id.bt_stop );
        mTvCurrentTime = findViewById( R.id.tv_currentTime );
        mTvTotalTime = findViewById( R.id.tv_totalTime );
    }
    private void initEvent() {
        mBtPause.setOnClickListener( this );
        mBtPlay.setOnClickListener( this );
        mBtStop.setOnClickListener( this );
        mFFmpegPlayer.setOnPlayerListener( new OnPlayerListener() {
            @Override
            public void OnLoad(boolean type) {
                if(type){
                    Log.i( "data", "OnLoad: >>>>加载中" );
                }else {
                    Log.i( "data", "OnLoad: >>>>播放中" );
                }
            }
            @Override
            public void OnPrepare() {
                mFFmpegPlayer.start();
                isStop = false;
            }

            @Override
            public void OnPause(boolean type) {
                isPause = type;
            }

            @Override
            public void onProgress(final int currentTime, final int totalTime) {
                runOnUiThread( new Runnable() {
                    @Override
                    public void run() {
                        mTvCurrentTime.setText( TimeUtils.secdsToDateFormat( currentTime,totalTime )+"/" );
                        mTvTotalTime.setText( TimeUtils.secdsToDateFormat( totalTime,totalTime ) );
                    }
                } );
            }
        } );
    }

    private void initData() {
        mFilePath = Environment.getExternalStorageDirectory()+File.separator+"space.mp3";
//        mFilePath = "http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3";
        mFFmpegPlayer = new FFPlayer();
    }
    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.bt_play:
                if(isStop){
                    if (ContextCompat.checkSelfPermission( this, Manifest.permission.WRITE_EXTERNAL_STORAGE ) != PackageManager.PERMISSION_GRANTED) {
                        String [] permission = {Manifest.permission.WRITE_EXTERNAL_STORAGE};
                        ActivityCompat.requestPermissions( this, permission, PERMISSION_CODE );
                    }else {
                        mFFmpegPlayer.setFilePath( mFilePath );
                        mFFmpegPlayer.prepare();
                    }
                }else if(isPause){
                        mFFmpegPlayer.play();
                }
                break;
            case R.id.bt_pause:
                mFFmpegPlayer.pause();
                break;
            case R.id.bt_stop:
                mFFmpegPlayer.stop();
                isStop = true;
                break;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult( requestCode, resultCode, data );
        switch (requestCode){
            case PERMISSION_CODE:
                mFFmpegPlayer.setFilePath( mFilePath );
                mFFmpegPlayer.prepare();
                break;
        }
    }
}
