package module.video.jnc.myffmpeg;

import android.opengl.GLSurfaceView;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by xhc on 2017/11/1.
 * http://blog.51cto.com/ticktick/1746136
 */

public class FFmpegUtils {
    static {
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
        System.loadLibrary("my_ffmpeg");
    }
//    public static native String stringNative();
//    //将MP4解码成yuv，和pcm
//    public static native int decodeMp4ToYuv(String path , Object glSurfaceView);
//    public static native int openSLTest(String path);
//    //播放或者暂停 false暂停，true播放
//    public static native int openSLPauseOrPlay(boolean flag);
//    public static native int openSLDestroy();
//    public static native int decodeMp4ToYuvShowShader(String path , Object glSurfaceView);
//    public static native int videoAudioDecodeShow(String path , Object glSurfaceView);
//    public static native int testMyShow();
//    public static native int testNativeThread();
//    public static native int testNativeThreadFree();
//    public static native int testNativeThreadRun();
//    public static native int testCPlusPlusThread();
//    public static native int showVideoGpuAudioOpensl(String path ,  Object glSurfaceView);
//    public static native int showVideoGpuDestroy();
//    public static native int showVideoGpuJustPause();
//    public static native int showVideoGpuPlayOrPause();
//    public static native int showVideoGpuSeek(double seek);
//    public static native int changeSpeedGpu(double speed);
//    //最大值一百，分别按比例处理
//    public static native int getPlayPosition();
//    public static native String getVersionString();
//
//    public static native int soundtouchStart();
//    public static native int soundtouchSpeed(double speed);
//    public static native int initsoundtouch(String path);

    private static List<Lis> listNativeNotify = new ArrayList<>();

    public static void addNativeNotify(Lis lis){
        listNativeNotify.add(lis);
    }

    public static void removeNotify(Lis lis){
        listNativeNotify.remove(lis);
    }

    interface Lis{
        void nativeNotify(String str);
    }

    //下面是做音视频播放器的
    public static native int initMp4Play(String path, Object glSurfaceView);

    public static native float getDuration();

    public static native int destroyMp4Play();

    public static native int mp4Pause();

    public static native int mp4Play();

    public static native int getProgress();

    public static native int changeSpeed(float speed);

    public static native int seekStart();
    public static native int seek(float progress);

    public static void nativeNotify(String str){
        for(Lis lis : listNativeNotify){
            lis.nativeNotify(str);
        }
    }

//    private void printMsgFromJni(String str) {
//        Log.e("xhc_jni", str);
////        return "x";
//    }
}
