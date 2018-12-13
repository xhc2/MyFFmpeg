package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;


public abstract class  MyGLSurfaceViewParent  extends GLSurfaceView {

    public MyGLSurfaceViewParent(Context context) {
        super(context);
    }

    public MyGLSurfaceViewParent(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public abstract void setPlayPath(String path);


}
