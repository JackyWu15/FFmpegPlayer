package com.hechuangwu.ffmpegplayer.utls;

/**
 * Created by cwh on 2019/3/1.
 * 功能:
 */
public class TimeUtils {
    public static String secdsToDateFormat(int currentTime, int totalTime) {
        long hours = currentTime / (60 * 60);
        long minutes = (currentTime % (60 * 60)) / (60);
        long seconds = currentTime % (60);

        String sh = "00";
        if (hours > 0) {
            if (hours < 10) {
                sh = "0" + hours;
            } else {
                sh = hours + "";
            }
        }
        String sm = "00";
        if (minutes > 0) {
            if (minutes < 10) {
                sm = "0" + minutes;
            } else {
                sm = minutes + "";
            }
        }

        String ss = "00";
        if (seconds > 0) {
            if (seconds < 10) {
                ss = "0" + seconds;
            } else {
                ss = seconds + "";
            }
        }
        if(totalTime >= 3600)
        {
            return sh + ":" + sm + ":" + ss;
        }
        return sm + ":" + ss;

    }
}
