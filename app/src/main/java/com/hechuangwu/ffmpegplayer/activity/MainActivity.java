package com.hechuangwu.ffmpegplayer.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.view.View;

import com.github.dfqin.grantor.PermissionListener;
import com.github.dfqin.grantor.PermissionsUtil;
import com.hechuangwu.ffmpegplayer.R;
import com.hechuangwu.ffmpegplayer.utls.Utils;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate( savedInstanceState );
        setContentView( R.layout.activity_main );
    }

    public void audio(View view) {
        if (PermissionsUtil.hasPermission( MainActivity.this, Manifest.permission.WRITE_EXTERNAL_STORAGE )) {
            startActivity( new Intent( this,AudioActivity.class ) );
        } else {
            PermissionsUtil.requestPermission( MainActivity.this, new PermissionListener() {
                @Override
                public void permissionGranted(@NonNull String[] permissions) {
                    startActivity( new Intent( MainActivity.this,AudioActivity.class ) );
                }

                @Override
                public void permissionDenied(@NonNull String[] permissions) {
                    Utils.showToast( MainActivity.this, "请开启存储权限" );
                }
            }, new String[]{Manifest.permission.RECORD_AUDIO} );
        }

    }

    public void video(View view) {
        if (PermissionsUtil.hasPermission( MainActivity.this, Manifest.permission.WRITE_EXTERNAL_STORAGE )) {
            startActivity( new Intent( this,VideoActivity.class ) );
        } else {
            PermissionsUtil.requestPermission( MainActivity.this, new PermissionListener() {
                @Override
                public void permissionGranted(@NonNull String[] permissions) {
                    startActivity( new Intent( MainActivity.this,VideoActivity.class ) );
                }

                @Override
                public void permissionDenied(@NonNull String[] permissions) {
                    Utils.showToast( MainActivity.this, "请开启存储权限" );
                }
            }, new String[]{Manifest.permission.RECORD_AUDIO} );
        }
    }
}
