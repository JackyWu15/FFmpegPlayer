package com.hechuangwu.ffmpegplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.hechuangwu.player.ffplayer.FfPlayer;
import com.hechuangwu.player.listener.OnPlayerListener;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private FfPlayer mFfPlayer;
    private Button mBtPlay;
    private static final String FILE_PATH = "http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3";
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
        mFfPlayer = new FfPlayer();
    }
    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.bt_play:
                mFfPlayer.setFilePath( FILE_PATH );
                mFfPlayer.prepare();
                break;
        }
    }
}
