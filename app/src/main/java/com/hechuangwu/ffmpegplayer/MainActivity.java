package com.hechuangwu.ffmpegplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;

import com.hechuangwu.player.ffplayer.FfPlayer;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private FfPlayer mFfPlayer;
    private Button mBtPlay;

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
        mBtPlay.setOnClickListener( new View.OnClickListener() {
            @Override
            public void onClick(View v) {

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
                mFfPlayer.prepare();
                break;

        }
    }
}
