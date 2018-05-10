package module.video.jnc.myffmpeg;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

/**
 * Created by xhc on 2018/4/25.
 */

public class MyVideoAudioSurfaceView  extends GLSurfaceView implements SurfaceHolder.Callback{

    public MyVideoAudioSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MyVideoAudioSurfaceView(Context context) {
        super(context);
    }

    @Override
    public void surfaceCreated(SurfaceHolder var1){
        new MyThread().start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4){

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder var1){

    }

    class MyThread extends Thread{
        @Override
        public void run() {
            super.run();
            //用真机sintel.ts 会解码出花屏的视频，但是把解码后的rgb存入文件中自己播放又没有问题。后面再看看是不是解码器的什么问题先继续走下去
            FFmpegUtils.videoAudioDecodeShow(/*Constant.rootFile.getAbsolutePath()+"/sintel.ts" */
                    Constant.rootFile.getAbsolutePath()+"/v1080.MP4", getHolder().getSurface());
        }
    }

}