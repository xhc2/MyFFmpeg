package module.video.jnc.myffmpeg;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import module.video.jnc.myffmpeg.opengl.Geometry;
import module.video.jnc.myffmpeg.opengl.NewMyRender;

public class OpenGlTureActivity extends AppCompatActivity {

    private GLSurfaceView glSurfaceView;
    private boolean renderSet ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);
//        setOnTouchEvent();
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




    private void setOnTouchEvent(){
        glSurfaceView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                if(motionEvent != null){
                    final float normalizedX = (motionEvent.getX() / (float) view.getWidth()) * 2 - 1;
                    final float normalizedY = -((motionEvent.getY() / (float) view.getHeight()) * 2 - 1);
                    if(motionEvent.getAction() == MotionEvent.ACTION_DOWN){
                        glSurfaceView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                handleTouchPress(normalizedX ,normalizedY );
                            }
                        });
                    }else if(motionEvent.getAction() == MotionEvent.ACTION_MOVE){
                        glSurfaceView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                handleTouchDrag(normalizedX , normalizedY);
                            }
                        });
                    }
                    return true;
                }
                return false;
            }
        });
    }


    public void handleTouchPress(float normalizedX , float normalizedY){

    }

    public void handleTouchDrag(float normalizedX , float normalizedY){

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
