package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.app.Dialog;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * 做一个视频播放器。
 * 1.播放音视频
 * 2.倍速播放
 * 3.seek功能。
 * 4.其他功能 (视频拼接，比如两个1s的视频拼接成2s的视频。又或者将画面拼接成一个画面的视频，水印等。)
 */

public class Mp4PlayerActivity extends Activity implements View.OnClickListener , AdapterView.OnItemClickListener{

    private static final int PLAY = 1;
    private static final int PAUSE = 2;

    private SeekBar seekBar;
    private TextView btPlay;
    private TextView tvSpeed;
    private boolean runFlag = false;
    private boolean pauseFlag = false;
    private PopupWindow popupWindow;
    private MyVideoGpuShow myVideoGpuShow;
    private int flag = PAUSE;
    private RelativeLayout rlTopBar;
    private RelativeLayout rlBottomBar;
    private int progress;
    private FileAdater adater;
    private List<File> listFile = new ArrayList<>();
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
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
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        rlTopBar = (RelativeLayout) findViewById(R.id.rl_topbar);
        rlBottomBar = (RelativeLayout) findViewById(R.id.rl_bottom_bar);

        myVideoGpuShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                rlTopBar.setVisibility(View.VISIBLE);
                rlBottomBar.setVisibility(View.VISIBLE);
                handler.removeCallbacksAndMessages(null);
                handler.sendEmptyMessageDelayed(1, 2000);
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
                showDialog();
            }
        });

        btPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                flag = PLAY == flag ? PAUSE : PLAY;
                if (flag == PLAY) {
                    if (FFmpegUtils.mp4Play() == 1) {
                        btPlay.setText("暂停");
                    } else {
                        flag = PAUSE;
                        Toast.makeText(Mp4PlayerActivity.this, "选择文件", Toast.LENGTH_SHORT).show();
                    }

                } else {
                    if (FFmpegUtils.mp4Pause() == 1) {
                        btPlay.setText("播放");
                    } else {
                        flag = PLAY;
                        Toast.makeText(Mp4PlayerActivity.this, "选择文件", Toast.LENGTH_SHORT).show();
                    }
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

    private void playVideo(String path){
        startThread();//Constant.rootFile.getAbsolutePath() + "/test.MP4"
        myVideoGpuShow.setPlayPath(path);
        btPlay.setText("暂停");
        flag = PLAY;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_0_5:
                FFmpegUtils.changeSpeed(0.5f);
                break;
            case R.id.tv_0_7:
                FFmpegUtils.changeSpeed(0.7f);
                break;
            case R.id.tv_1_0:
                FFmpegUtils.changeSpeed(1.0f);
                break;
            case R.id.tv_1_2:
                FFmpegUtils.changeSpeed(1.2f);
                break;
            case R.id.tv_1_5:
                FFmpegUtils.changeSpeed(1.5f);
                break;
            case R.id.tv_1_7:
                FFmpegUtils.changeSpeed(1.7f);
                break;
            case R.id.tv_2_0:
                FFmpegUtils.changeSpeed(2.0f);
                break;
        }
        if (popupWindow != null && popupWindow.isShowing()) {
            popupWindow.dismiss();
        }

    }

    private Dialog dialog;

    private void getFileList() {
        File[] files = Constant.rootVideoFile.listFiles();
        if (files != null) {
            for (File f : files) {
                listFile.add(f);
            }
        }
    }


    private void createDialog() {
        if (dialog == null) {
            dialog = new Dialog(this  , R.style.dialog);
            dialog.setContentView(R.layout.dialog_listview_layout);
            getFileList();
            adater = new FileAdater();
            ListView listView = dialog.findViewById(R.id.listview);
            listView.setAdapter(adater);
            listView.setOnItemClickListener(this);
        }
    }

    private void showDialog() {
        createDialog();
        if (!dialog.isShowing()) {
            dialog.show();
        }
    }

    private void dismissDialog() {
        if (dialog != null && dialog.isShowing()) {
            dialog.dismiss();
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
                progress = FFmpegUtils.getProgress();
                seekBar.setProgress(progress);
            }
        }
    }

    private class FileAdater extends BaseAdapter {

        @Override
        public int getCount() {
            return listFile.size();
        }

        @Override
        public Object getItem(int i) {
            return listFile.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
             ViewHolder holder;
            if (view == null) {
                holder = new  ViewHolder();
                holder.tv = new TextView(Mp4PlayerActivity.this);
                holder.tv.setPadding(10, 10, 10, 10);
                ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                holder.tv.setLayoutParams(params);
                view = holder.tv;
                view.setTag(holder);
            } else {
                holder = ( ViewHolder) view.getTag();
            }
            holder.tv.setText(listFile.get(i).getName());
            return view;
        }

        class ViewHolder {
            TextView tv;
        }
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        FFmpegUtils.destroyMp4Play();
        stopThread();
        File file = listFile.get(position);
        playVideo(file.getAbsolutePath());
        dismissDialog();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopThread();
        FFmpegUtils.destroyMp4Play();
    }
}
