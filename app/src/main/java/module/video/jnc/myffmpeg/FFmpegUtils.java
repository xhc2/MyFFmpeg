package module.video.jnc.myffmpeg;

import android.opengl.GLSurfaceView;
import android.text.TextUtils;
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

    private static List<Lis> listNativeNotify = new ArrayList<>();

    public static void addNativeNotify(Lis lis){
        listNativeNotify.add(lis);
    }

    public static void removeNotify(Lis lis){
        listNativeNotify.remove(lis);
    }

    public interface Lis{
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
    public static native int getVideoWidth();
    public static native int getVideoHeight();


    //rtmp推流部分
    public static native int rtmpInit(String outPath , String inputPath);
    public static native int rtmpClose();

    //通过手机摄像头推送rtmp
    public static native int rtmpCameraInit(String outPath , int width , int height , int pcmSize);
    public static native int rtmpCameraStream(byte[] bytes);
    public static native int rtmpAudioStream(byte[] bytes , int size);
    public static native int rtmpDestroy();
    public static native int startRecord();
    public static native int pauseRecord();


    //srs_lib_rtmp
    public static native int srsTest(String path);
    public static native int srsDestroy();

    //flv
    public static native String flvParse(String path);

    //h264
    public static native String h264Parse(String path);
    public static native byte[] getNextNalu(String path);

    //aac
    public static native String aacParse(String path);
    public static native  byte[]  getAACFrame(String path);

    //视频剪辑
    public static native void startClip(String path , String output ,   int start , int end);
    public static native int getClipProgress();
    public static native void destroyClip();

    //视频拼接
    public static native void startJoint(String[] paths , String output  , int outWidth , int outHeight);
    public static native int getJointProgress();
    public static native void destroyJoint();


    //opengl test
    public static native void openGlTest(String path, Object glSurfaceView);
    public static native void openDestroy();

    //视频倒放
    public static native void startBackRun(String inputPath , String output);
    public static native void destroyBackRun();

    //获取当前时间图片
    public static native int initCurrentBitmp(String path ,   int outWidth , int outHeight );
    public static native float getCurrentBitmp(float time  , byte[] result);
    public static native void destroyCurrentBitmap();

    //视频打图片水印部分
//    public static native void initBitmapWaterMark( String videoPath  ,String outputPath ,  String filterDescr);
//    public static native int bitmapWaterMarkStart();
//    public static native int getWaterMarkProgress();
//    public static native int bitmapWaterMarkDestroy();

    /**
     * @param params 作以下规定 -1 ，代表默认
     *               int[] = {
     *                      width , height
     *               }
     *               e.g ,列如第一个参数就是width ， 第二个参数是height
     *               int[] = {
     *                     640 , 360
     *               }
     *
     */
    public static native void initVideoFilter( String videoPath  ,String outputPath ,  String filterDescr , int[] params);
    public static native int videoFilterStart();
    public static native int getVideoFilterProgress();
    public static native int videoFilterDestroy();


    //视频裁剪相关
////    public static native int initVideoCrop(String videoPath , String outputPath , int x , int y , int w , int h);
////    public static native int startCrop();
////    public static native int getCropProgress();
//    public static native int destroyCrop();


    //测试部分
    public static native int test();
    public static native int test2();


    //给本地回调，是在jni层调用的方法
    public static void nativeNotify(String str){
        for(Lis lis : listNativeNotify){
            lis.nativeNotify(str);
        }
    }
    //判断本地回调的数据是否是用来显示的
    public static boolean isShowToastMsg(String msg){
        if(TextUtils.isEmpty(msg)){
            return false;
        }
        if(msg.startsWith("metadata")){
            return false;
        }
        return true;
    }
}
