package module.video.jnc.myffmpeg;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

/**
 * Created by xhc on 2018/4/24.
 */

public class MyGLSurfaceShader extends GLSurfaceView implements SurfaceHolder.Callback {

    public MyGLSurfaceShader(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MyGLSurfaceShader(Context context) {
        super(context);
    }

    @Override
    public void surfaceCreated(SurfaceHolder var1) {
        new MyGLSurfaceShader.MyThread().start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder var1) {

    }

    class MyThread extends Thread {
        @Override
        public void run() {
            super.run();
            FFmpegUtils.decodeMp4ToYuvShowShader(Constant.rootFile.getAbsolutePath() + "/test_480_272.yuv", getHolder().getSurface());
        }
    }

}