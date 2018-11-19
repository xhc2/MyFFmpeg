package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;
import java.util.ArrayList;
import java.util.List;
import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;


public class VideoClipActivity extends VideoEditParentActivity   {

    private MyVideoGpuShow myVideoGpuShow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_deal_video);
        titleBar = findViewById(R.id.title);
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                showDialog("loading");
            }
        });
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);

    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus && listPath.size() > 0){
            startThread();
        }
    }
    private StartPlayThraed playThread;
    private void stopThread(){
    }
    private void startThread(){
        stopThread();
        playThread = new StartPlayThraed();
        playThread.start();
    }

    class StartPlayThraed extends Thread{
        @Override
        public void run() {
            super.run();
            synchronized (VideoClipActivity.class){
                FFmpegUtils.destroyMp4Play();
                playVideo(listPath.get(0));
            }

        }
    }

    private void startClip(int startSecond , int endSecond){
        if(listPath.size() > 0){
            FFmpegUtils.startClip(listPath.get(0) , "sdcard/FFmpeg/ClipOutput.mp4" ,startSecond , endSecond );
        }

    }


    private void playVideo(String path) {
        myVideoGpuShow.setPlayPath(path);
//        startThread();
//        btPlay.setText("暂停");
//        flag = PLAY;
//        setTime(0);
    }



    @Override
    protected void onResume() {
        super.onResume();
        myVideoGpuShow.onResume();

    }

    @Override
    protected void onPause() {
        super.onPause();
        myVideoGpuShow.onPause();
//        flag = PAUSE;
//        pauseFlag = true;
//        if (FFmpegUtils.mp4Pause() == 1) {
//            btPlay.setText("播放");
//        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.destroyClip();
        FFmpegUtils.destroyMp4Play();
    }
}
