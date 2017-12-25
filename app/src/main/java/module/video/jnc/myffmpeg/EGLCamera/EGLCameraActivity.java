package module.video.jnc.myffmpeg.EGLCamera;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.opengl.NewMyRender;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class EGLCameraActivity extends AppCompatActivity implements SurfaceTexture.OnFrameAvailableListener{

    private GLSurfaceView glSurfaceView ;
    private MyCamera myCamera;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_eglcamera);
        myCamera = new MyCamera();
        glSurfaceView = findViewById(R.id.surface_view);
        ActivityManager activityManager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        if(!(configurationInfo.reqGlEsVersion >= 0x20000)){
            //不支持

        }
        else{
            //支持opengl 2
            glSurfaceView.setEGLContextClientVersion(2);
            glSurfaceView.setEGLConfigChooser(8,8,8,8,16,0);
            glSurfaceView.setRenderer(new MyCameraRender(this , this , myCamera));
            glSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
        }
    }


    @Override
    protected void onPause() {
        super.onPause();
        myCamera.stop();
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        glSurfaceView.requestRender();
    }
}
