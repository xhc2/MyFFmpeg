package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import module.video.jnc.myffmpeg.MyRender;

public class MyGlSurfaceViewCommon extends GLSurfaceView implements SurfaceHolder.Callback {


    public MyGlSurfaceViewCommon(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.setRenderer(new MyRender());
//        this.getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        super.surfaceCreated(holder);
        holder.addCallback(this);
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
