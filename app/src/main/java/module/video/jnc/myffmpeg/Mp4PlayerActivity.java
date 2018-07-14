package module.video.jnc.myffmpeg;

import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

/**
 * 做一个视频播放器。
 * 1.播放音视频
 * 2.倍速播放
 * 3.seek功能。
 * 4.其他功能 (视频拼接，比如两个1s的视频拼接成2s的视频。又或者将画面拼接成一个画面的视频，水印等。)
 */

public class Mp4PlayerActivity extends AppCompatActivity implements View.OnClickListener {

    private static final int PLAY = 1;
    private static final int PAUSE = 2;

    private SeekBar seekBar;
    private TextView btPlay;
    private TextView tvSpeed;
    private boolean runFlag = false;
    private boolean pauseFlag = false;
    private PopupWindow popupWindow;
    private MyVideoGpuShow myVideoGpuShow;
    private int flag = PAUSE ;
    private RelativeLayout rlTopBar;
    private RelativeLayout rlBottomBar;
    private int progress;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what){
                case 1:
                    rlTopBar.setVisibility(View.GONE);
                    rlBottomBar.setVisibility(View.GONE);
                    break;
            }
            return false;
        }
    });
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_mp4_player);
        seekBar = (SeekBar) findViewById(R.id.seek_bar);
        btPlay = (TextView) findViewById(R.id.bt_play_button);
        tvSpeed = (TextView) findViewById(R.id.bt_play_speed);
        myVideoGpuShow = (MyVideoGpuShow)findViewById(R.id.play_gl_surfaceview);
        rlTopBar = (RelativeLayout)findViewById(R.id.rl_topbar);
        rlBottomBar =  (RelativeLayout)findViewById(R.id.rl_bottom_bar);

        startThread();
        myVideoGpuShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rlTopBar.setVisibility(View.VISIBLE);
                rlBottomBar.setVisibility(View.VISIBLE);
                handler.removeCallbacksAndMessages(null);
                handler.sendEmptyMessageDelayed(1 , 2000);
            }
        });
        tvSpeed.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (popupWindow == null || !popupWindow.isShowing()) {
                    popWindowShow();
                } else {
                    popupWindow.dismiss();
                }
            }
        });

        findViewById(R.id.ib_more).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                myVideoGpuShow.setPlayPath(Constant.rootFile.getAbsolutePath() + "/test.MP4");
            }
        });

        btPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                flag = PLAY == flag ? PAUSE : PLAY;
                if (flag == PLAY) {
                    btPlay.setText("暂停");
                    FFmpegUtils.mp4Play();
                } else {
                    btPlay.setText("播放");
                    FFmpegUtils.mp4Pause();
                }
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                pauseFlag = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                pauseFlag = false;
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_0_5:
                break;
            case R.id.tv_0_7:
                break;
            case R.id.tv_1_0:
                break;
            case R.id.tv_1_2:
                break;
            case R.id.tv_1_5:
                break;
            case R.id.tv_1_7:
                break;
            case R.id.tv_2_0:
                break;
        }
        if (popupWindow != null && popupWindow.isShowing()) {
            popupWindow.dismiss();
        }

    }

    private void popWindowShow() {
        if (popupWindow == null) {
            View view = LayoutInflater.from(this).inflate(R.layout.speed_layout, null, false);
            view.findViewById(R.id.tv_0_5).setOnClickListener(this);
            view.findViewById(R.id.tv_0_7).setOnClickListener(this);
            view.findViewById(R.id.tv_1_0).setOnClickListener(this);
            view.findViewById(R.id.tv_1_2).setOnClickListener(this);
            view.findViewById(R.id.tv_1_5).setOnClickListener(this);
            view.findViewById(R.id.tv_1_7).setOnClickListener(this);
            view.findViewById(R.id.tv_2_0).setOnClickListener(this);

            popupWindow = new PopupWindow(DensityUtils.dip2px(this, 40), DensityUtils.dip2px(this, 210));
            popupWindow.setContentView(view);
            popupWindow.setOutsideTouchable(true);
        }
        popupWindow.showAsDropDown(tvSpeed, 0, 0);
    }


    PositionThread thread;

    private void startThread() {
        stopThread();
        runFlag = true;
        thread = new PositionThread();
        thread.start();
    }

    private void stopThread() {
        runFlag = false;
        thread = null;
    }

    class PositionThread extends Thread {

        @Override
        public void run() {
            super.run();

            while (runFlag) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
//                if (pauseFlag) {
//                    continue;
//                }
                progress = FFmpegUtils.getProgress();
                Log.e("xhc" ," progress "+progress);
                seekBar.setProgress(progress);
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopThread();
        FFmpegUtils.destroyMp4Play();
    }
}
