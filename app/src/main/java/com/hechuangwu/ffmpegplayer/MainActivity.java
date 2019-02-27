package com.hechuangwu.ffmpegplayer;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.hechuangwu.player.ffplayer.FfPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private FfPlayer mFfPlayer;
    private Button mBtPlay;
    private final static int PERMISSION_CODE = 1;
    private String mFilePath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        initView();
        initData();
        initEvent();
    }
    private void initView() {
        setContentView( R.layout.activity_main );
        mBtPlay = findViewById( R.id.bt_play );
    }
    private void initEvent() {
        mBtPlay.setOnClickListener( this );
        mFfPlayer.setOnPlayerListener( new OnPlayerListener() {
            @Override
            public void OnPrepare() {
                mFfPlayer.start();
            }
        } );
    }

    private void initData() {
        mFilePath = Environment.getExternalStorageDirectory()+File.separator+"space.mp3";
        mFfPlayer = new FfPlayer();
    }
    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.bt_play:
                if (ContextCompat.checkSelfPermission( this, Manifest.permission.WRITE_EXTERNAL_STORAGE ) != PackageManager.PERMISSION_GRANTED) {
                    String [] permission = {Manifest.permission.WRITE_EXTERNAL_STORAGE};
                    ActivityCompat.requestPermissions( this, permission, PERMISSION_CODE );
                }else {
                    mFfPlayer.setFilePath( mFilePath );
                    mFfPlayer.prepare();
                }

                break;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult( requestCode, resultCode, data );
        switch (requestCode){
            case PERMISSION_CODE:
                mFfPlayer.setFilePath( mFilePath );
                mFfPlayer.prepare();
                break;
        }
    }
}
