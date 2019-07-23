package com.hechuangwu.player.ffplayer;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.hechuangwu.player.ChannelEnum;
import com.hechuangwu.player.listener.OnPlayerListener;
import com.hechuangwu.player.log.MyLog;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by cwh on 2019/2/15.
 * 功能:
 */
public class FFPlayer {
    static {
        System.loadLibrary( "ffplayer" );
        System.loadLibrary( "avcodec-57" );
        System.loadLibrary( "avdevice-57" );
        System.loadLibrary( "avfilter-6" );
        System.loadLibrary( "avformat-57" );
        System.loadLibrary( "avutil-55" );
        System.loadLibrary( "postproc-54" );
        System.loadLibrary( "swresample-2" );
        System.loadLibrary( "swscale-4" );
    }

    private String filePath;
    private OnPlayerListener onPlayerListener;
    private int totalTime;
    private int volumePercent = 100;
    private ChannelEnum channel = ChannelEnum.LEFT;
    private float pitch = 1.0f;
    private float tempo = 1.0f;
    private FileOutputStream mFileOutputStream;


    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public void setOnPlayerListener(OnPlayerListener onPlayerListener) {
        this.onPlayerListener = onPlayerListener;
    }


    public void onPrepareCallBack() {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.OnPrepare();
        }
    }

    public void onProgressCallBack(int currentTime, int totalTime) {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.onProgress( currentTime, totalTime );
            this.totalTime = totalTime;
        }
    }

    public int getTotalTime() {
        return this.totalTime;
    }

    public void onErrorCallBack(int type, String msg) {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.onError( type, msg );
        }
    }


    public void prepare() {
        if (!TextUtils.isEmpty( filePath )) {
            new Thread( new Runnable() {
                @Override
                public void run() {
                    _prepare( filePath );
                }
            } ).start();
        } else {
            MyLog.e( "filePath is empty!" );
        }
    }

    public void onLoadCallBack(boolean type) {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.OnLoad( type );
        }
    }

    public void onPauseCallBack(boolean type) {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.OnPause( type );
        }
    }

    public void onCompleteCallBack() {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.onComplete();
        }
    }

    public void onPCMDBCallBack(int db) {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.onPCMDB( db );
        }
    }

    public void start() {
        if (!TextUtils.isEmpty( filePath )) {
            new Thread( new Runnable() {
                @Override
                public void run() {
                    seekVolume( volumePercent );
                    setTempo( tempo );
                    setPitch( pitch );
                    setChannel( channel );
                    setVolumePercent( volumePercent );
                    _start();
                }
            } ).start();
        } else {
            MyLog.e( "filePath is empty!" );
        }
    }

    public void pause() {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.OnPause( true );
        }
        _pause();
    }

    public void play() {
        if (this.onPlayerListener != null) {
            this.onPlayerListener.OnPause( false );
        }
        _play();
    }

    public void stop() {
        new Thread( new Runnable() {
            @Override
            public void run() {
                _stop();
            }
        } ).start();

    }

    public void seek(int seconds) {
        _seek( seconds );
    }

    public void seekVolume(int percent) {
        if (percent >= 0 && percent <= 100) {
            this.volumePercent = percent;
            _setVolume( percent );
        }
    }

    public void setVolumePercent(int volumePercent) {
        seekVolume( volumePercent );
    }

    public int getVolumePercent() {
        return volumePercent;
    }

    public void setPitch(float pitch) {
        this.pitch = pitch;
        _setPitch( pitch );
    }

    public void setTempo(float tempo) {
        this.tempo = tempo;
        _setTempo( tempo );
    }

    public void setChannel(ChannelEnum channelEnum) {
        channel = channelEnum;
        _setChannel( channelEnum.getType() );
    }


    public void videoPrepare() {
        if (!TextUtils.isEmpty( filePath )) {
            new Thread( new Runnable() {
                @Override
                public void run() {
                    _video_prepare( filePath );
                }
            } ).start();
        } else {
            MyLog.e( "filePath is empty!" );
        }
    }

    public void videoStart(Surface surface) {
        _video_start( surface );
    }


    private native void _prepare(String filePath);

    private native void _start();

    private native void _pause();

    private native void _play();

    private native void _stop();

    private native void _seek(int seconds);

    private native void _setVolume(int percent);

    private native void _setChannel(int channel);

    private native void _setPitch(float pitch);

    private native void _setTempo(float tempo);


    private native void _video_prepare(String filePath);

    private native void _video_start(Surface surface);


    private int audioSampleRate = 0;
    private double recordTime = 0;
    private MediaCodec mEncoder;
    private MediaCodec.BufferInfo mMediaInfo;
    private int perPCMSize = 0;
    private byte[] outByteBuffer = null;
    private int mAdtSsamplerate;
    private boolean isInitMediaCodec;


    public void startRecord(File outFile) {
        if (!isInitMediaCodec) {
            audioSampleRate = _sampleRate();
            if (audioSampleRate > 0) {
                isInitMediaCodec = true;
                initMediaCodec( audioSampleRate, outFile );
                _startOrStopRecord( true );
            }
        }
    }

    public void stopRecord() {
        if (isInitMediaCodec) {
            isInitMediaCodec = false;
            _startOrStopRecord( false );
            releaseMediaCodec();
        }
    }

    private void initMediaCodec(int sampleRate, File outFile) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                mAdtSsamplerate = getADTSsamplerate( sampleRate );
                MediaFormat audioFormat = MediaFormat.createAudioFormat( MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, 2 );
                audioFormat.setInteger( MediaFormat.KEY_BIT_RATE, 96000 );
                audioFormat.setInteger( MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC );
                audioFormat.setInteger( MediaFormat.KEY_MAX_INPUT_SIZE, 4096 );//一次输入的数据大小

                mEncoder = MediaCodec.createEncoderByType( MediaFormat.MIMETYPE_AUDIO_AAC );
                mMediaInfo = new MediaCodec.BufferInfo();
                if (mEncoder != null) {
                    recordTime = 0;
                    mEncoder.configure( audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE );
                    mFileOutputStream = new FileOutputStream( outFile );
                    mEncoder.start();
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    private void encodecPCMToAAC(int size, byte[] buffer) {
        Log.i( "data", "encodecPCMToAAC: " + size + ">>>>>>>>>>" + buffer );
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN) {
            if (buffer != null && mEncoder != null) {
                recordTime += (size * 1.0) / (audioSampleRate * 2 * 2);//1秒audioSampleRate*2*2字节
                if (this.onPlayerListener != null) {
                    this.onPlayerListener.onRecordTime( (int) recordTime );
                }

                //获取一个可使用的缓冲区
                int dequeueInputBufferIndex = mEncoder.dequeueInputBuffer( 0 );
                if (dequeueInputBufferIndex >= 0) {
                    ByteBuffer inputBuffer = mEncoder.getInputBuffers()[dequeueInputBufferIndex];
                    inputBuffer.clear();
                    inputBuffer.put( buffer );//存入缓冲区
                    mEncoder.queueInputBuffer( dequeueInputBufferIndex, 0, size, 0, 0 );//入队
                }

                int index = mEncoder.dequeueOutputBuffer( mMediaInfo, 0 );//帧出队，填充媒体信息,返回缓冲区角标
                while (index >= 0) {
                    try {
                        perPCMSize = 7 + mMediaInfo.size;//adts头+数据
                        outByteBuffer = new byte[perPCMSize];
                        addADtsHeader( outByteBuffer, perPCMSize, mAdtSsamplerate );//对块添加ADTS头

                        //获取处理后的缓冲数据
                        ByteBuffer outputBuffer = mEncoder.getOutputBuffers()[index];
                        outputBuffer.position( mMediaInfo.offset );//设置读取开始位置

                        outputBuffer.limit( mMediaInfo.offset + mMediaInfo.size );//读取限制，避免越界

                        //对块添加aac数据
                        outputBuffer.get( outByteBuffer, 7, mMediaInfo.size );
                        outputBuffer.position( mMediaInfo.offset );

                        mFileOutputStream.write( outByteBuffer, 0, perPCMSize );//写入文件
                        mEncoder.releaseOutputBuffer( index, false );//释放缓冲区
                        index = mEncoder.dequeueOutputBuffer( mMediaInfo, 0 );//一帧一次不一定取完，需再判断一次
                        outByteBuffer = null;

                    } catch (IOException e) {

                        e.printStackTrace();
                    }
                }
            }

        }

    }

    private void releaseMediaCodec() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            if (mEncoder == null) {
                return;
            }
            try {
                recordTime = 0;
                mFileOutputStream.close();
                mFileOutputStream = null;
                mEncoder.stop();

                mEncoder.release();

                mEncoder = null;
                isInitMediaCodec = false;
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (mFileOutputStream != null) {
                    try {
                        mFileOutputStream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    mFileOutputStream = null;
                }
            }
        }
    }

    private native int _sampleRate();

    private native void _startOrStopRecord(boolean start);

    private void addADtsHeader(byte[] packet, int packetLen, int samplerate) {
        int profile = 2; // AAC LC
        int freqIdx = samplerate; // samplerate
        int chanCfg = 2; // CPE

        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个t位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSsamplerate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }
}
