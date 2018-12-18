package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoMusicActivity extends VideoEditParentActivity {

    private String musicPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_music);
        findViewById();
        init();
    }


    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
    }

    protected void init() {
        super.init();
        musicPath = getIntent().getStringExtra("music_path");
        if(TextUtils.isEmpty(musicPath)){
            showToast("音乐文件错误！");
            finish();
        }
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("正在处理...");
                    return;
                }
                FileUtils.makeMusicVideo();
                startVideoMusic();
                showLoadPorgressDialog("正在处理...");
            }
        });
    }


    private VideoMusicThread videoMusicThread;

    private void startVideoMusic() {
        stopVideoMusic();
        startProgressThread();
        if (videoMusicThread == null) {
            videoMusicThread = new VideoMusicThread();
            videoMusicThread.start();
        }
    }

    private void stopVideoMusic() {
        destroyFFmpeg();
        if (videoMusicThread != null) {
            try {
                videoMusicThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        videoMusicThread = null;
    }

    class VideoMusicThread extends Thread {
        @Override
        public void run() {
            super.run();
            dealFlag = true;
            FFmpegUtils.initVideoMusic(listPath.get(0), musicPath, FileUtils.APP_MUSIC_VIDEO + System.currentTimeMillis() + ".mp4");
            dealFlag = false;
        }
    }


    @Override
    protected int getProgress() {
        return FFmpegUtils.videoMusicProgress();
    }

    @Override
    protected int destroyFFmpeg() {
        FFmpegUtils.destroyVideoMusic();
        return 1;
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(0));
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopVideoMusic();

    }
}
