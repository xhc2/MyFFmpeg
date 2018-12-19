package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoSpeedActivity extends VideoEditParentActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_speed);
        findViewById();
        init();
    }

    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
    }

    protected void init() {
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("正在处理...");
                    return;
                }
                FileUtils.makeSpeedDir();
                startSpeed();
                showLoadPorgressDialog("正在处理...");
            }
        });
    }


    private SpeedThread speedThread;
    private void stopSpeed(){
        destroyFFmpeg();
        if(speedThread != null){
            try {
                speedThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        speedThread = null;
    }
    private void startSpeed(){
        stopSpeed();
        startProgressThread();
        if(speedThread == null){
            speedThread = new SpeedThread();
            speedThread.start();
        }
    }

    private class SpeedThread extends Thread{
        @Override
        public void run() {
            super.run();
            dealFlag = true;
            FFmpegUtils.initVideoSpeed(listPath.get(0) , 0.6f , FileUtils.APP_SPEED+System.currentTimeMillis()+".mp4");
            dealFlag =  false;
        }
    }

    @Override
    protected int getProgress() {
        return FFmpegUtils.videoSpeedProgress();
    }

    @Override
    protected int destroyFFmpeg() {
        FFmpegUtils.destroyVideoSpeed();
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
        stopSpeed();
    }
}
