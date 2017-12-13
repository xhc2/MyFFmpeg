package module.video.jnc.myffmpeg;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import module.video.jnc.myffmpeg.opengl.NewMyRender;

public class OpenGlTureActivity extends AppCompatActivity {

    private GLSurfaceView glSurfaceView;
    private boolean renderSet ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);
        setContentView(glSurfaceView);
        ActivityManager activityManager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
        if(!(configurationInfo.reqGlEsVersion >= 0x20000)){
            Log.e("xhc" , " 不支持");
        }
        else{
            Log.e("xhc" , " 支持  ");
            glSurfaceView.setEGLContextClientVersion(2);
            glSurfaceView.setEGLConfigChooser(8,8,8,8,16,0);
            glSurfaceView.setRenderer(new NewMyRender(this));
            renderSet = true;
        }



    }

    @Override
    protected void onPause() {
        super.onPause();
        if(renderSet){
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(renderSet){
            glSurfaceView.onResume();
        }
    }
}
