package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.ClipBar;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;


public class VideoClipActivity extends VideoEditParentActivity implements ClipBar.TouchCallBack {

    private MyVideoGpuShow myVideoGpuShow;
    private int clipProgress;
    private ClipBar clipBar;
    private TextView tvStart;
    private TextView tvEnd;
    private int startTime ;
    private int endTime;

    private final static int PROGRESS = 0;

    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what){
                case PROGRESS:
                    if(msg.arg1 == 100){
                        dismissLoadPorgressDialog();
                        showToast("已完成");
                        break;
                    }
                    setLoadPorgressDialogProgress(msg.arg1);
                    break;
            }
            return false;
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_deal_video);
        findViewById();
        init();
        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.getCurrentBitmp(listPath.get(0), 8.5f, 300, 300);
            }
        });
    }



    private void findViewById() {
        clipBar = findViewById(R.id.clip_bar);
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        tvStart = findViewById(R.id.tv_start);
        tvEnd = findViewById(R.id.tv_end);
    }

    private void init() {
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                showLoadPorgressDialog("请稍等...");
                startClip(startTime, endTime);
                startProgressThread();
                FFmpegUtils.getCurrentBitmp(listPath.get(0), 8.5f, 300, 300);
            }
        });
        clipBar.setTouchCallBack(this);
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        Log.e("xhc" , "  onWindowFocusChanged  ");
        if (hasFocus && listPath.size() > 0) {
//            startPlayThread();
        }
    }

    private StartPlayThraed playThread;

    private void startPlayThread() {
        playThread = new StartPlayThraed();
        playThread.start();
    }

    class StartPlayThraed extends Thread {
        @Override
        public void run() {
            super.run();
            synchronized (VideoClipActivity.class) {
                FFmpegUtils.destroyMp4Play();
                playVideo(listPath.get(0));
            }
        }
    }

    private ClipThread clipThread;

    private void startClip(int startSecond, int endSecond) {
        if (clipThread == null) {
            clipThread = new ClipThread(startSecond, endSecond);
            clipThread.start();
        }
    }

    class ClipThread extends Thread {
        int startSecond;
        int endSecond;

        ClipThread(int startSecond, int endSecond) {
            this.startSecond = startSecond;
            this.endSecond = endSecond;
        }

        @Override
        public void run() {
            super.run();
            if (listPath.size() > 0) {
                FFmpegUtils.startClip(listPath.get(0), "sdcard/FFmpeg/ClipOutput.mp4", startSecond, endSecond);
            }
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
        }
        progressThread = null;
    }

    class ProgressThread extends Thread {
        boolean progressFlag = false;

        @Override
        public void run() {
            super.run();
            while (progressFlag) {
                clipProgress = FFmpegUtils.getClipProgress();
                Log.e("xhc" , " clipprogress "+clipProgress);
                Message msg = handler.obtainMessage();
                msg.what = PROGRESS;
                msg.arg1 = clipProgress;
                handler.sendMessage(msg);
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

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
        stopProgressThread();
        FFmpegUtils.destroyClip();
        FFmpegUtils.destroyMp4Play();
    }

    @Override
    public void moveStart(float screenStartX, int startProgress) {
        tvStart.setText("开始时间： "+((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
    }

    @Override
    public void moveEnd(float screenEndX, int endProgress) {
        tvEnd.setText("结束时间： "+((float)endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
    }

    @Override
    public void moveFinish(int startProgress, int endProgress) {
        Log.e("xhc", " startProgress " + startProgress + " endProgress " + endProgress);
        startTime =  (int)(((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        endTime =  (int)(((float)endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        tvStart.setText("开始时间： "+ startTime);
        tvEnd.setText("结束时间： "+ endTime);
    }
}
