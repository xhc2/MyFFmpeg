package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class VideoEditParentActivity extends BaseActivity {
    protected List<String> listPath = new ArrayList<>();
    protected TitleBar titleBar;
    protected MyVideoGpuShow myVideoGpuShow;
    protected boolean activityFoucsFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        ArrayList<String> tempVideos = intent.getStringArrayListExtra("videos");
        if (tempVideos != null) {
            listPath.addAll(tempVideos);
        }


    }

    protected void init() {
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    //播放的线程
    private StartPlayThraed playThread;

    protected void startPlayThread(String path) {
        playThread = new StartPlayThraed(path);
        playThread.start();
    }

    protected void stopPlayThread() {
        FFmpegUtils.destroyMp4Play();
        if (playThread != null) {
            try {
                playThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    class StartPlayThraed extends Thread {

        String playPath;

        StartPlayThraed(String playPath) {
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopPlayThread();
        FFmpegUtils.destroyMp4Play();
    }
}
