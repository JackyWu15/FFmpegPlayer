package com.hechuangwu.ffmpegplayer.activity;

import android.Manifest;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.UserHandle;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import com.hechuangwu.ffmpegplayer.R;
import com.hechuangwu.ffmpegplayer.utls.Utils;
import com.hechuangwu.player.ChannelEnum;
import com.hechuangwu.player.ffplayer.FFPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

import java.io.File;

public class AudioActivity extends Activity implements View.OnClickListener {


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
    private SeekBar mSbCurrent;
    private SeekBar mSb_volume;
    private TextView mTv_volume;
    private SeekBar mSb_tempo;
    private TextView mTv_tempo;
    private TextView mTv_db;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        initView();
        initData();
        initEvent();
    }
    private void initView() {
        setContentView( R.layout.activity_audio );
        mBtPause = findViewById( R.id.bt_pause );
        mBtPlay = findViewById( R.id.bt_play );
        mBtStop = findViewById( R.id.bt_stop );
        mSbCurrent = findViewById( R.id.sb_current );
        mTvCurrentTime = findViewById( R.id.tv_currentTime );
        mTvTotalTime = findViewById( R.id.tv_totalTime );
        mSb_volume = findViewById( R.id.sb_volume );
        mTv_volume = findViewById( R.id.tv_volume );
        mTv_tempo = findViewById( R.id.tv_tempo );
        mSb_tempo = findViewById( R.id.sb_tempo );
        mTv_db = findViewById( R.id.tv_db );
    }
    private void initEvent() {
        mBtPause.setOnClickListener( this );
        mBtPlay.setOnClickListener( this );
        mBtStop.setOnClickListener( this );
        mSbCurrent.setMax( 100 );
        mSbCurrent.setProgress( 0 );
        mSb_tempo.setMax( 20 );
        mSb_tempo.setProgress( 10 );

        mSbCurrent.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mFFmpegPlayer.seek( seekBar.getProgress() );
            }
        } );
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
            public void onError(int type, String msg) {
                Log.i( "data", "onError: "+type+">>>>"+msg );
            }
            @Override
            public void onComplete() {
                Log.i( "data", "onComplete: >>>>>>>>>播放完成" );
            }

            @Override
            public void onPCMDB(final int db) {
                runOnUiThread( new Runnable() {
                    @Override
                    public void run() {
                        Log.i( "data", "分贝值为: "+db );
                        mTv_db.setText( "分贝："+db );
                    }
                } );

            }

            @Override
            public void onProgress(final int currentTime, final int totalTime) {
                runOnUiThread( new Runnable() {
                    @Override
                    public void run() {
                        mTvCurrentTime.setText( Utils.secdsToDateFormat( currentTime,totalTime )+"/" );
                        mTvTotalTime.setText( Utils.secdsToDateFormat( totalTime,totalTime ) );
                    }
                } );
            }

            @Override
            public void onRecordTime(int recordTime) {

            }

        } );


        mSb_volume.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mFFmpegPlayer.seekVolume(progress);
                mTv_volume.setText("音量：" + mFFmpegPlayer.getVolumePercent() + "%");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        } );

        mSb_tempo.setOnSeekBarChangeListener( new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                float v = progress * 0.1f;
                mFFmpegPlayer.setTempo( v );
                mTv_tempo.setText("速度：" + v );

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        } );
    }

    private void initData() {
//        mFilePath = Environment.getExternalStorageDirectory()+ File.separator+"Space O.mp3";
                mFilePath = "http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3";
        mFFmpegPlayer = new FFPlayer();
        mFFmpegPlayer.setFilePath( mFilePath );
        mFFmpegPlayer.setChannel( ChannelEnum.CENTER );
        mFFmpegPlayer.setPitch( 1.0f );
        mFFmpegPlayer.setTempo( 1.0f );
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


    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {

    }

    public void left(View view) {
        mFFmpegPlayer.setChannel( ChannelEnum.LEFT );
    }

    public void right(View view) {
        mFFmpegPlayer.setChannel( ChannelEnum.RIGHT );
    }

    public void center(View view) {
        mFFmpegPlayer.setChannel( ChannelEnum.CENTER );
    }


    public void recordRecord(View view) {
        mFFmpegPlayer.startRecord( new File( Environment.getExternalStorageDirectory()+File.separator+"ffmpegRecord.aac" ) );
    }

    public void stopRecord(View view) {
        mFFmpegPlayer.stopRecord();
    }
}
