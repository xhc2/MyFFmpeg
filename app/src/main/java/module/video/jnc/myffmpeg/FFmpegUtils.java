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
        System.loadLibrary("avfilter");
        System.loadLibrary("ffmpeg");
    }


    public static native String stringNative();

    public static native String stringJni();

    public static native int encode(String inFilePath , String outputPath);

    public static native int decode(String filePath , String outputStr);

    public static native int stream(String inputUrl , String ouputUrl);

}
