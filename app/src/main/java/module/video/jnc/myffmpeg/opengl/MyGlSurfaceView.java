package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import module.video.jnc.myffmpeg.FFmpegUtils;

public class MyGlSurfaceView extends GLSurfaceView implements SurfaceHolder.Callback{


    public MyGlSurfaceView(Context context) {
        super(context);
    }

    public MyGlSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void start(){
        new Thread(){
            @Override
            public void run() {
                super.run();
                FFmpegUtils.openGlTest("sdcard/FFmpeg/oneframe.yuv" , getHolder().getSurface());
            }
        }.start();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        super.surfaceCreated(holder);


    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        super.surfaceChanged(holder, format, w, h);
    }
}
