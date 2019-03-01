package com.hechuangwu.ffmpegplayer.activity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.hechuangwu.ffmpegplayer.R;
import com.hechuangwu.ffmpegplayer.utls.TimeUtils;
import com.hechuangwu.player.ffplayer.FFPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private FFPlayer mFFmpegPlayer;
    private Button mBtPlay;
    private final static int PERMISSION_CODE = 1;
    private String mFilePath;
    private Button mBtStart;
    private Button mBtPause;
    private TextView mTvCurrentTime;
    private TextView mTvTotalTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        initView();
        initData();
        initEvent();
    }
    private void initView() {
        setContentView( R.layout.activity_main );
        mBtStart = findViewById( R.id.bt_start );
        mBtPause = findViewById( R.id.bt_pause );
        mBtPlay = findViewById( R.id.bt_play );
        mTvCurrentTime = findViewById( R.id.tv_currentTime );
        mTvTotalTime = findViewById( R.id.tv_totalTime );
    }
    private void initEvent() {
        mBtStart.setOnClickListener( this );
        mBtPause.setOnClickListener( this );
        mBtPlay.setOnClickListener( this );
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
            }

            @Override
            public void OnPause(boolean type) {
                if(type){
                    Log.i( "data", "OnPause: >>>>暂停中" );
                }else {
                    Log.i( "data", "OnPause: >>>>播放" );
                }
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
            case R.id.bt_start:
                if (ContextCompat.checkSelfPermission( this, Manifest.permission.WRITE_EXTERNAL_STORAGE ) != PackageManager.PERMISSION_GRANTED) {
                    String [] permission = {Manifest.permission.WRITE_EXTERNAL_STORAGE};
                    ActivityCompat.requestPermissions( this, permission, PERMISSION_CODE );
                }else {
                    mFFmpegPlayer.setFilePath( mFilePath );
                    mFFmpegPlayer.prepare();
                }
                break;
            case R.id.bt_pause:
                mFFmpegPlayer.pause();
                break;
            case R.id.bt_play:
                mFFmpegPlayer.play();
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
