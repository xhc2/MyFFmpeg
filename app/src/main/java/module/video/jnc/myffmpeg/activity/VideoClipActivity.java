package module.video.jnc.myffmpeg.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.ClipBar;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

//现在有个问题。就是获取当前时间帧有点问题。效果总是有点不理想，先放着吧。

public class VideoClipActivity extends VideoEditParentActivity implements ClipBar.TouchCallBack {

    private MyVideoGpuShow myVideoGpuShow;
    private int clipProgress;
    private ClipBar clipBar;
    private TextView tvStart;
    private TextView tvEnd;
    private int startTime = -1;
    private int endTime = -1;
    private int outWidth = 100;
    private int outHeight = 100;
    private final static int BMP_HEAD = 54;
    private final static int PROGRESS = 0;
    private final static int GETCURRENTBITMAP = 1;
    private boolean activityFoucsFlag = false;
    private byte[] buffer = new byte[outWidth * outHeight * 3 + BMP_HEAD];
    private ImageView img;

    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case PROGRESS:
                    if (msg.arg1 == 100) {
                        dismissLoadPorgressDialog();
                        showToast("已完成");
                        stopProgressThread();
                        break;
                    }
                    setLoadPorgressDialogProgress(msg.arg1);
                    break;
                case GETCURRENTBITMAP:

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

//        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
////                FFmpegUtils.getCurrentBitmp(listPath.get(0), 8.5f, 300, 300);
//                FFmpegUtils.getCurrentBitmp( startTime , buffer);
//                try {
//
//                    img.setImageBitmap(BitmapFactory.decodeByteArray(buffer , 0 , buffer.length));
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
//            }
//        });
    }

    private void findViewById() {
        clipBar = findViewById(R.id.clip_bar);
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        tvStart = findViewById(R.id.tv_start);
        tvEnd = findViewById(R.id.tv_end);
        img = findViewById(R.id.img);
    }

    private void init() {
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                if (clipFlag) {
                    showToast("正在裁剪中，请稍等");
                    return;
                }
                if (startTime == -1 || endTime == -1 || startTime >= endTime || startTime < 0 || endTime > FFmpegUtils.getDuration()) {
                    showToast("裁剪时间有问题！");
                    return;
                }
                showLoadPorgressDialog("处理中...");
                startClip(startTime, endTime);
                startProgressThread();
            }
        });
        clipBar.setTouchCallBack(this);
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
        FFmpegUtils.initCurrentBitmp(listPath.get(0), outWidth, outHeight);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread();
        }
    }

    //播放的线程
    private StartPlayThraed playThread;
    private void startPlayThread() {
        playThread = new StartPlayThraed();
        playThread.start();
    }
    private void stopPlayThread() {
        if (playThread != null) {
            try {
                playThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
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


    //具体裁剪的线程
    private ClipThread clipThread;
    private boolean clipFlag = false;
    private void startClip(int startSecond, int endSecond) {
        if (clipThread == null) {
            clipThread = new ClipThread(startSecond, endSecond);
            clipThread.start();
        }
    }
    private void stopClipThread() {
        if (clipThread != null) {
            try {
                clipThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            clipThread = null;
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
                clipFlag = true;
                FFmpegUtils.startClip(listPath.get(0), "sdcard/FFmpeg/ClipOutput.mp4", startSecond, endSecond);
                clipFlag = false;
            }
        }
    }

    //查看剪辑的进度线程
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
                clipProgress = FFmpegUtils.getClipProgress();
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


    //获取缩略图的线程池
    ExecutorService es = Executors.newSingleThreadExecutor();

    class CurrentBitmapThread extends Thread {

        private float startTime;

        public CurrentBitmapThread(float startTime) {
            this.startTime = startTime;
        }

        @Override
        public void run() {
            super.run();
            synchronized (VideoClipActivity.class) {
                float time = FFmpegUtils.getCurrentBitmp(startTime, buffer);
                Log.e("xhc", " time " + time + " startTime " + startTime);
                if (startTime == time) {
                    handler.sendEmptyMessage(GETCURRENTBITMAP);
                }
            }
        }
    }

    @Override
    public void onBackPressed() {
        Log.e("xhc" , " onbackPress");
        if(clipFlag){
            showAlertDialog(null, "放弃裁剪?", new DialogInterface.OnClickListener() {
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

    private void playVideo(String path) {
        myVideoGpuShow.setPlayPath(path);
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
    }


    @Override
    public void moveStart(float screenStartX, int startProgress) {
        float startTime = ((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration();
        tvStart.setText("开始时间： " + startTime);
//        es.execute(new CurrentBitmapThread(startTime));
//        img.setX(screenStartX);
//        img.setImageBitmap(BitmapFactory.decodeByteArray(buffer, 0, buffer.length));
    }

    @Override
    public void moveEnd(float screenEndX, int endProgress) {
        float endTime = ((float) endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration();
        tvEnd.setText("结束时间： " + endTime);
    }

    @Override
    public void moveFinish(int startProgress, int endProgress) {
        Log.e("xhc", " startProgress " + startProgress + " endProgress " + endProgress);
        startTime = (int) (((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        endTime = (int) (((float) endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        tvStart.setText("开始时间： " + startTime);
        tvEnd.setText("结束时间： " + endTime);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopProgressThread();
        stopPlayThread();
        stopClipThread();
        FFmpegUtils.destroyClip();
        FFmpegUtils.destroyMp4Play();
        FFmpegUtils.destroyCurrentBitmap();
        es.shutdown();
        handler.removeCallbacksAndMessages(null);
    }

}
