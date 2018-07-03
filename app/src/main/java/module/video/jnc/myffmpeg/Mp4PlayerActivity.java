package module.video.jnc.myffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.PopupWindow;
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

    private SeekBar seekBar;
    private TextView btPlay;
    private TextView tvSpeed;
    private boolean runFlag = false;
    private boolean pauseFlag = false;
    private PopupWindow popupWindow;

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
//        startThread();

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

        btPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int flag = FFmpegUtils.showVideoGpuPlayOrPause();
                if (flag == 1) {
                    btPlay.setText("播放");
                } else {
                    btPlay.setText("暂停");
                }
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                FFmpegUtils.showVideoGpuJustPause();
                pauseFlag = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                FFmpegUtils.showVideoGpuSeek((double) seekBar.getProgress() / (double) seekBar.getMax());
                pauseFlag = false;
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_0_5:
                FFmpegUtils.changeSpeedGpu(0.5f);
                break;
            case R.id.tv_0_7:
                FFmpegUtils.changeSpeedGpu(0.7f);
                break;
            case R.id.tv_1_0:
                FFmpegUtils.changeSpeedGpu(1.0f);
                break;
            case R.id.tv_1_2:
                FFmpegUtils.changeSpeedGpu(1.2f);
                break;
            case R.id.tv_1_5:
                FFmpegUtils.changeSpeedGpu(1.5f);
                break;
            case R.id.tv_1_7:
                FFmpegUtils.changeSpeedGpu(1.7f);
                break;
            case R.id.tv_2_0:
                FFmpegUtils.changeSpeedGpu(2.0f);
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
                if (pauseFlag) {
                    continue;
                }
                int position = FFmpegUtils.getPlayPosition();
                seekBar.setProgress(position);
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
        Log.e("xhc", " ondestroy ");
        stopThread();
        FFmpegUtils.showVideoGpuDestroy();
    }
}
