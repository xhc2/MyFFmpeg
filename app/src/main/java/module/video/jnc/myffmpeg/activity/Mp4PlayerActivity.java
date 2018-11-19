package module.video.jnc.myffmpeg.activity;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.Constant;
import module.video.jnc.myffmpeg.tool.DensityUtils;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

/**
 * 做一个视频播放器。
 * 1.播放音视频
 * 2.倍速播放
 * 3.seek功能。
 * 4.rtmp 推流，服务器先用red5 就可以
 * 5.其他功能 (视频拼接，比如两个1s的视频拼接成2s的视频。又或者将画面拼接成一个画面的视频，水印等。)
 * http://ffmpeg.org/doxygen/3.4/index.html
 */

public class Mp4PlayerActivity extends Activity implements View.OnClickListener,
        AdapterView.OnItemClickListener, FFmpegUtils.Lis {

    private static final int PLAY = 1;
    private static final int PAUSE = 2;

    private SeekBar seekBar;
    private TextView btPlay;
    private TextView tvSpeed;
    private TextView tvTime;
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
    private float videoDuration;
    private String playPath;

    private Handler handler = new Handler(new Handler.Callback() {

        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    if (flag == PLAY) {
//                        rlTopBar.setVisibility(View.GONE);
//                        rlBottomBar.setVisibility(View.GONE);
                    }
                    break;
                case 2:
                    setTime(progress);
                    seekBar.setProgress(progress);
                    break;
                case 3:
                    String path = (String) msg.obj;
                    playVideo(path);
                    break;
                case 4:
                    String str = (String) msg.obj;
                    Toast.makeText(Mp4PlayerActivity.this, str, Toast.LENGTH_LONG).show();
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
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(new MyRender());//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);


        findViewById();

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

        findViewById(R.id.ib_hard).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(Mp4PlayerActivity.this, HardCodeActivity.class));
            }
        });

        findViewById(R.id.ib_publish).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //跳转到网路流的界面
                Intent intent = new Intent(Mp4PlayerActivity.this, NetStreamActivity.class);
                startActivity(intent);
            }
        });
//        findViewById(R.id.ib_video_cart).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent(Mp4PlayerActivity.this, VideoEditActivity.class);
//                startActivity(intent);
//            }
//        });


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

        findViewById(R.id.ib_flv).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(Mp4PlayerActivity.this , FlvParseActivity.class));
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                pauseFlag = true;
                FFmpegUtils.seekStart();
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                float pro = (float) (seekBar.getProgress() * 1.0 / 100);
                FFmpegUtils.seek(pro);
                pauseFlag = false;
            }
        });
        FFmpegUtils.addNativeNotify(this);


        Intent intent = getIntent();
        playPath = intent.getStringExtra("path");


    }

    private void findViewById(){
        seekBar = (SeekBar) findViewById(R.id.seek_bar);
        btPlay = (TextView) findViewById(R.id.bt_play_button);
        tvSpeed = (TextView) findViewById(R.id.bt_play_speed);

        rlTopBar = (RelativeLayout) findViewById(R.id.rl_topbar);
        tvTime = (TextView) findViewById(R.id.time);
        rlBottomBar = (RelativeLayout) findViewById(R.id.rl_bottom_bar);
    }


    /**
     * 可以用来判断activity是否被加载完毕
     * @param hasFocus
     */
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if(hasFocus && !TextUtils.isEmpty(playPath)){
            playVideo(playPath);
        }
    }

    private void playVideo(String path) {
        myVideoGpuShow.setPlayPath(path);
        startThread();
        btPlay.setText("暂停");
        flag = PLAY;
        setTime(0);
    }

    private void setTime(int curProgress) {
        videoDuration = FFmpegUtils.getDuration();
        if (videoDuration <= 0 || curProgress < 0) {
            tvTime.setText(String.format(getString(R.string.time), "00:00", "00:00"));
            return;
        }

        String vd = videoDuration + "";
        double curD = Math.ceil(curProgress * 1.0 / 100 * videoDuration);
        String cur = String.valueOf(curD);
        tvTime.setText(String.format(getString(R.string.time), cur, vd));
    }



    @Override
    public void onClick(View v) {
        float speed = 0;
        switch (v.getId()) {
            case R.id.tv_0_5:
                speed = 0.5f;
                break;
            case R.id.tv_0_7:
                speed = 0.5f;
                break;
            case R.id.tv_1_0:
                speed = 1.0f;
                break;
            case R.id.tv_1_2:
                speed = 1.2f;
                break;
            case R.id.tv_1_5:
                speed = 1.5f;
                break;
            case R.id.tv_1_7:
                speed = 1.7f;
                break;
            case R.id.tv_2_0:
                speed = 2.0f;
                break;
        }
        if (popupWindow != null && popupWindow.isShowing()) {
            popupWindow.dismiss();
        }
        FFmpegUtils.changeSpeed(speed);
        tvSpeed.setText(speed + "X");
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
            dialog = new Dialog(this, R.style.dialog);
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


    private PositionThread thread;

    private void startThread() {
        stopThread();
        runFlag = true;
        thread = new PositionThread();
        thread.start();
    }

    private void stopThread() {
        runFlag = false;
        try {
            thread.join();
        } catch (Exception e) {

        }
        thread = null;
    }

    class PositionThread extends Thread {

        @Override
        public void run() {
            super.run();

            while (runFlag) {
                if (pauseFlag) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    continue;
                }
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                progress = FFmpegUtils.getProgress();
                handler.sendEmptyMessage(2);
            }
        }
    }

    private class FileAdater extends BaseAdapter {

        @Override
        public int getCount() {
            return listFile.size() + 1;
        }

        @Override
        public Object getItem(int i) {
            return null;
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder holder;
            if (view == null) {
                holder = new ViewHolder();
                holder.tv = new TextView(Mp4PlayerActivity.this);
                holder.tv.setPadding(10, 10, 10, 10);
                ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                holder.tv.setLayoutParams(params);
                view = holder.tv;
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            if (i == 0) {
                holder.tv.setText("rtmp://live.hkstv.hk.lxdns.com/live/hks");
            } else {
                holder.tv.setText(listFile.get(i - 1).getName());
            }

            return view;
        }

        class ViewHolder {
            TextView tv;
        }
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
        dismissDialog();
        stopThread();
        new Thread() {
            @Override
            public void run() {
                super.run();
                FFmpegUtils.destroyMp4Play();
                Message msg = new Message();
                msg.what = 3;
                if (position == 0) {
                    msg.obj = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
                } else {
                    msg.obj = listFile.get(position - 1).getAbsolutePath();
                }
                handler.sendMessage(msg);
            }
        }.start();
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
        flag = PAUSE;
        pauseFlag = true;
        if (FFmpegUtils.mp4Pause() == 1) {
            btPlay.setText("播放");
        }
    }

    @Override
    public void nativeNotify(String str) {
        Message msg = new Message();
        msg.what = 4;
        msg.obj = str;
        handler.sendMessage(msg);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopThread();
        FFmpegUtils.removeNotify(this);
        FFmpegUtils.destroyMp4Play();
    }



}
