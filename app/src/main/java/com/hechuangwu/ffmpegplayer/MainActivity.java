package com.hechuangwu.ffmpegplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import com.hechuangwu.player.ffplayer.FfPlayer;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.activity_main );

        // Example of a call to a native method
        TextView tv = (TextView) findViewById( R.id.sample_text );
        tv.setText( new FfPlayer().stringFromJNI()  );
    }

}
