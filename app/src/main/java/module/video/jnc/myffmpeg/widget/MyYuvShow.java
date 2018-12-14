package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.tool.FileUtils;


public class MyYuvShow extends MyGLSurfaceViewParent implements SurfaceHolder.Callback {


    public MyYuvShow(Context context) {
        super(context);
    }

    public MyYuvShow(Context context, AttributeSet attrs) {
        super(context, attrs);
    }


    @Override
    public void setPlayPath(String path) {
        FileUtils.makeDubDir();
        new MyThread(path).start();
    }

    @Override
    public void surfaceCreated(SurfaceHolder var1) {
    }

    @Override
    public void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder var1) {

    }


    class MyThread extends Thread {
        String path;

        MyThread(String path) {
            this.path = path;
        }

        @Override
        public void run() {
            super.run();
            FFmpegUtils.initVideoDub(path, FileUtils.APP_DUB + /*System.currentTimeMillis()+*/"a.flv" , getHolder().getSurface() );
        }
    }
}
