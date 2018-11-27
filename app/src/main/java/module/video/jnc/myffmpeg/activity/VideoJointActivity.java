package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class VideoJointActivity extends VideoEditParentActivity {

    private boolean activityFoucsFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_joint);
        findViewById();
        init();

    }


    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
    }

    private void init() {
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
            }
        });
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
//            startPlayThread();
        }
    }

    //播放的线程
    private StartPlayThraed playThread;
    private void startPlayThread(String path) {
        playThread = new StartPlayThraed( path);
        playThread.start();
    }
    private void stopPlayThread() {
        if (playThread != null) {
            try {
                FFmpegUtils.destroyMp4Play();
                playThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    class StartPlayThraed extends Thread {

        String playPath;
        StartPlayThraed(String playPath){
            this.playPath = playPath;
        }

        @Override
        public void run() {
            super.run();
            synchronized (VideoClipActivity.class) {

                playVideo(this.playPath);
            }
        }
    }

    private void playVideo(String path) {
        myVideoGpuShow.setPlayPath(path);
    }

}
