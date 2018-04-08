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
//    Cannot resolve corresponding JNI function Java_module_video_jnc_myffmpeg_FFmpegUtils_init_1audio_1record_1 less... (Ctrl+F1)
//    Reports native method declarations in Java where no corresponding JNI function is found in the project.

    //方法名最好不要有下划线，不然ndk编译有问题，还不知道为什么。
    public static native int initAudioRecord(String outputPath , int aSize);

    public static native int encodeAudioRecord(byte[] bytes);

    public static native int closeAudioRecord();

    public static native String stringNative();

    public static native String stringJni();

    public static native int encode(String inFilePath , String outputPath);

    public static native int decode(String filePath , String outputStr);

    public static native int stream(String inputUrl , String ouputUrl);

    public static native int encodeYuv(String inputUrl , String outputPath);

//    public static native int transcodeing(String inputUrl , String outputPath);

    public static native int addfilter(String inputStr , String outputPath);

    public static native int swscale(String inputUrl , String outputPath);

    public static native int muxer(String output , String inputVPath , String inputAPath);

    public static native int demuxer(String inputUrl , String outputVPath , String outputAPath);

    public static native int myInit(String outputPath , int width , int height);

    public static native int nv21ToYv12(byte[] bytes);

    public static native int encodePcm(byte[] bytes , int size);

    public static native int testArray(byte[] bytes);

    public static native void closeMyFFmpeg();

    public static native void encodeCamera(byte[] bytes);

    public static native int initMyCameraMuxer(String outputPath , int width , int height , int aSize);

    public static native int encodeMyMuxerCamera(byte[] bytes);

    public static native int encodeMyMuxerAudio(byte[] bytes);

    public static native int closeMyMuxer();

    public static native int filterCameraInit(String outputPath , int width , int height , int aSize);

    public static native int encodeMyMuxerCameraFilter(byte[] bytes);

    public static native int encodeMyMuxerAudioFilter(byte[] bytes);

    public static native int closeMyMuxerFilter();

    public static native int jniNativeThreadStart();

    public static native int jniNativeThreadStop();

}
