package module.video.jnc.myffmpeg;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

/**
 * Created by xhc on 2018/5/10.
 */

public class MyVideoGpuShow extends GLSurfaceView implements SurfaceHolder.Callback {

    public MyVideoGpuShow(Context context) {
        super(context);
    }

    public MyVideoGpuShow(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void surfaceCreated(SurfaceHolder var1) {
        new MyThread().start();
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
            FFmpegUtils.showVideoGpuAudioOpensl(Constant.rootFile.getAbsolutePath() + "/test.MP4", getHolder().getSurface());
        }
    }


}
