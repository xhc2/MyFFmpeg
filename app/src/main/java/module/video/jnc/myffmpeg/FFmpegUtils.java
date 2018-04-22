package module.video.jnc.myffmpeg;

import android.opengl.GLSurfaceView;
import android.util.Log;

/**
 * Created by xhc on 2017/11/1.
 */

public class FFmpegUtils {

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avfilter");
        System.loadLibrary("my_ffmpeg");
    }
    public static native String stringNative();

    //将MP4解码成yuv，和pcm
    public static native int decodeMp4ToYuvPcm(String path , Object glSurfaceView);


//    public static native int initAudioRecord(String outputPath , int aSize);
//
//    public static native int encodeAudioRecord(byte[] bytes);
//
//    public static native int closeAudioRecord();
//

//
//    public static native String stringJni();
//
//    public static native int encode(String inFilePath , String outputPath);
//
//    public static native int decode(String filePath , String outputStr);
//
//    public static native int stream(String inputUrl , String ouputUrl);
//
//    public static native int encodeYuv(String inputUrl , String outputPath);
//
//    public static native int addfilter(String inputStr , String outputPath);
//
//    public static native int swscale(String inputUrl , String outputPath);
//
//    public static native int muxer(String output , String inputVPath , String inputAPath);
//
//    public static native int demuxer(String inputUrl , String outputVPath , String outputAPath);
//
//    public static native int myInit(String outputPath , int width , int height);
//
//    public static native int nv21ToYv12(byte[] bytes);
//
//    public static native int encodePcm(byte[] bytes , int size);
//
//    public static native int testArray(byte[] bytes);
//
//    public static native void closeMyFFmpeg();
//
//    public static native void encodeCamera(byte[] bytes);
//
//    public static native int initMyCameraMuxer(String outputPath , int width , int height , int aSize);
//
//    public static native int encodeMyMuxerCamera(byte[] bytes);
//
//    public static native int encodeMyMuxerAudio(byte[] bytes);
//
//    public static native int closeMyMuxer();
//
//    public static native int filterCameraInit(String outputPath , int width , int height , int aSize);
//
//    public static native int encodeMyMuxerCameraFilter(byte[] bytes);
//
//    public static native int encodeMyMuxerAudioFilter(byte[] bytes);
//
//    public static native int closeMyMuxerFilter();


    private void printMsgFromJni(String str){
        Log.e("xhc_jni" ,str);
//        return "x";
    }
}
