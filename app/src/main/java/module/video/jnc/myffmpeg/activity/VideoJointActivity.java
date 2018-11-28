package module.video.jnc.myffmpeg.activity;

import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class VideoJointActivity extends VideoEditParentActivity {

    private boolean activityFoucsFlag = false;
    private int playProgress;
    private int jointProgress;
    private int playCount;
    private static final String outPath = "sdcard/FFmpeg/videojoint.flv";
    private int outWidth;
    private int outHeight;
    private boolean joinFlag  ;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case 2:
                    if (playProgress == -100) {
                        //播放下一个
                        if (listPath.size() > (playCount + 1)) {
                            playCount++;
                            stopPlayThread();
                            startPlayThread(listPath.get(playCount));
                        }
                    }
                    break;
                case 3:
                    if(jointProgress == 100){
                        //拼接完成
                        stopJointProgress();
                        dismissLoadPorgressDialog();
                        showToast("拼接完成!");
                        finish();
                    }
                    setLoadPorgressDialogProgress(jointProgress);
                    break;
            }
            return false;
        }
    });

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
                if (joinFlag) {
//                    showToast("正在裁剪中，请稍等");
                    showLoadPorgressDialog("正在拼接...");
                    return;
                }
                startJointThread();
                startJointProgress();
                showLoadPorgressDialog("正在拼接...");
            }
        });
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
        outWidth = 1280;
        outHeight = 720;
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(playCount));
            startProgressThread();
        }
    }


    //合并视频的线程
    private JointThread jointThread;
    private void startJointThread(){
        jointThread = new JointThread();
        jointThread.start();
    }
    private void stopJointThread(){
        FFmpegUtils.destroyJoint();
        if(jointThread != null){
            try {
                jointThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    class JointThread extends Thread{
        @Override
        public void run() {
            super.run();
            joinFlag = true;
            String path[] = new String[listPath.size()];
            listPath.toArray(path);
            FFmpegUtils.startJoint(path , outPath, outWidth, outHeight);
            joinFlag = false;
        }
    }

    JointProgressThread jointProgressThread ;
    private void startJointProgress(){
        stopJointProgress();
        jointProgressThread = new JointProgressThread();
        jointProgressThread.runFlag = true;
        jointProgressThread.start();
    }

    private void stopJointProgress(){
        if(jointProgressThread != null){
            jointProgressThread.runFlag = false;
            try {
                jointProgressThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            jointProgressThread = null;
        }


    }
    class JointProgressThread extends Thread{

        boolean runFlag ;

        @Override
        public void run() {
            super.run();
            while(runFlag){
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                jointProgress = FFmpegUtils.getJointProgress();
                handler.sendEmptyMessage(3);
                Log.e("xhc"  , " joint Prgress "+jointProgress);
            }
        }
    }

    //获取播放进度
    private PositionThread thread;
    private void startProgressThread() {
        stopProgressThread();
        thread = new PositionThread();
        thread.runFlag = true;
        thread.start();
    }

    private void stopProgressThread() {
        if (thread == null) {
            return;
        }
        thread.runFlag = false;
        try {
            thread.join();
        } catch (Exception e) {

        }
        thread = null;
    }

    class PositionThread extends Thread {
        boolean runFlag;

        @Override
        public void run() {
            super.run();
            while (runFlag) {


                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                playProgress = FFmpegUtils.getProgress();
                Log.e("xhc", " progress  " + playProgress);
                handler.sendEmptyMessage(2);
            }
        }
    }

    //播放的线程
    private StartPlayThraed playThread;

    private void startPlayThread(String path) {
        playThread = new StartPlayThraed(path);
        playThread.start();
    }

    private void stopPlayThread() {
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
    public void onBackPressed() {
        Log.e("xhc" , " onbackPress");
        if(joinFlag){
            showAlertDialog(null, "放弃拼接?", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dismissLoadPorgressDialog();
                    dialog.dismiss();
                    finish();
                }
            });
        }
        else{
            super.onBackPressed();
        }
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopPlayThread();
        stopProgressThread();
        stopJointThread();
        stopJointProgress();
    }
}
