package module.video.jnc.myffmpeg;

/**
 * Created by xhc on 2017/11/1.
 */

public class FFmpegUtils {

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avdevice");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
//        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter");
        System.loadLibrary("ffmpeg");
//        System.loadLibrary("hello-jni");
    }


    public static native String stringNative();


    public static native String stringJni();


}
