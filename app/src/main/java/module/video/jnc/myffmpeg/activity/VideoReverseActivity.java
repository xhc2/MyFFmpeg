package module.video.jnc.myffmpeg.activity;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoReverseActivity extends VideoEditParentActivity {

    private final static int PROGRESS = 2;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what){
                case PROGRESS:
                    if (progress == 100) {
                        dismissLoadPorgressDialog();
                        showToast("已完成");
                        stopProgressThread();
                        FFmpegUtils.destroyBackRun();
                        break;
                    }
                    setLoadPorgressDialogProgress(progress);
                    break;
            }
            return false;
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_reverse);
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
                FileUtils.makeReverse();
                startReverse();
                showLoadPorgressDialog("正在处理...");
            }
        });
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(0));
        }
    }



    private ProgressThread progressThread;
    private void startProgressThread() {
        stopProgressThread();
        progressThread = new ProgressThread();
        progressThread.progressFlag = true;
        progressThread.start();
    }

    private void stopProgressThread() {
        if (progressThread != null) {
            progressThread.progressFlag = false;
            try {
                progressThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            progressThread = null;
        }
    }

    class ProgressThread extends Thread {
        boolean progressFlag = false;

        @Override
        public void run() {
            super.run();
            while (progressFlag) {
                progress = FFmpegUtils.getBackRunProgress();
                handler.sendEmptyMessage(PROGRESS);
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    ReverseThread reverseThread ;
    private void startReverse(){
        stopReverse();
        startProgressThread();
        if(reverseThread == null){
            reverseThread = new ReverseThread();
            reverseThread.start();
        }
    }
    private void stopReverse(){
        FFmpegUtils.destroyBackRun();
        if(reverseThread != null){
            try {
                reverseThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        reverseThread= null;
    }
    class ReverseThread extends Thread{
        @Override
        public void run() {
            super.run();
            dealFlag = true;
            FFmpegUtils.startBackRun(listPath.get(0), FileUtils.APP_REVERSE+"_"+System.currentTimeMillis()+".mp4");
            dealFlag = false;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopProgressThread();
        stopReverse();
    }
}
