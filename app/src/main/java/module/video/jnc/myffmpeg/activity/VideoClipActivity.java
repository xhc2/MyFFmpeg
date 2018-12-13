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
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.ClipBar;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

//现在有个问题。就是获取当前时间帧有点问题。效果总是有点不理想，先放着吧。

public class VideoClipActivity extends VideoEditParentActivity implements ClipBar.TouchCallBack {

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
    private byte[] buffer = new byte[outWidth * outHeight * 3 + BMP_HEAD];
    private ImageView img;

//    private Handler handler = new Handler(new Handler.Callback() {
//        @Override
//        public boolean handleMessage(Message msg) {
//            switch (msg.what) {
//                case PROGRESS:
//                    if (msg.arg1 == 100) {
//                        dismissLoadPorgressDialog();
//                        showToast("已完成");
//                        stopClipThread();
//                        stopProgressThread();
//                        break;
//                    }
//                    setLoadPorgressDialogProgress(msg.arg1);
//                    break;
//                case GETCURRENTBITMAP:
//
//                    break;
//            }
//            return false;
//        }
//    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_deal_video);
        findViewById();
        init();

    }

    private void findViewById() {
        clipBar = findViewById(R.id.clip_bar);
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        tvStart = findViewById(R.id.tv_start);
        tvEnd = findViewById(R.id.tv_end);
        img = findViewById(R.id.img);
    }

    protected void init() {
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                FileUtils.makeClipDir();
                if (dealFlag) {
                    showToast("正在裁剪中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
                if (startTime == -1 || endTime == -1 || startTime >= endTime || startTime < 0 || endTime > FFmpegUtils.getDuration()) {
                    showToast("裁剪时间有问题！");
                    return;
                }
                showLoadPorgressDialog("处理中...");
                startClip(startTime, endTime);

            }
        });
        clipBar.setTouchCallBack(this);

        FFmpegUtils.initCurrentBitmp(listPath.get(0), outWidth, outHeight);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(0));
        }
    }




    //具体裁剪的线程
    private ClipThread clipThread;
    private void startClip(int startSecond, int endSecond) {
        if (clipThread == null) {
            clipThread = new ClipThread(startSecond, endSecond);
            clipThread.start();
        }
        startProgressThread();
    }
    private void stopClipThread() {
        FFmpegUtils.destroyClip();
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
                dealFlag = true;
                FFmpegUtils.startClip(listPath.get(0), FileUtils.APP_CLIP+"clip_"+System.currentTimeMillis()+".mp4", startSecond, endSecond);
                dealFlag = false;
            }
        }
    }




    //获取缩略图的线程池
//    ExecutorService es = Executors.newSingleThreadExecutor();
//
//    class CurrentBitmapThread extends Thread {
//
//        private float startTime;
//
//        public CurrentBitmapThread(float startTime) {
//            this.startTime = startTime;
//        }
//
//        @Override
//        public void run() {
//            super.run();
//            synchronized (VideoClipActivity.class) {
//                float time = FFmpegUtils.getCurrentBitmp(startTime, buffer);
//                Log.e("xhc", " time " + time + " startTime " + startTime);
//                if (startTime == time) {
//                    handler.sendEmptyMessage(GETCURRENTBITMAP);
//                }
//            }
//        }
//    }


    @Override
    protected int getProgress(){
        return FFmpegUtils.getClipProgress();
    }

    @Override
    protected int destroyFFmpeg(){
        FFmpegUtils.destroyClip();
        return 1;
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
        stopClipThread();

        FFmpegUtils.destroyMp4Play();
        FFmpegUtils.destroyCurrentBitmap();
//        es.shutdown();
//        handler.removeCallbacksAndMessages(null);
    }

}
