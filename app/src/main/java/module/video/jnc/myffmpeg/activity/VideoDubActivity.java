package module.video.jnc.myffmpeg.activity;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.MyYuvShow;

/**
 * 只播放视频
 * 然后用户自己配音。
 */
public class VideoDubActivity extends VideoEditParentActivity {

    private Button btStart;
    private AudioRecord audioRecord;
    private static final int sampleRate = 44100;
    private static final int pcmFormat = AudioFormat.ENCODING_PCM_16BIT;
    private static final int channel = AudioFormat.CHANNEL_IN_MONO;
    private int pcmSize = 4096;
    private boolean flag;
    private byte[] bytes;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_dub);
        findViewById();
        init();
    }


    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyYuvShow) findViewById(R.id.play_gl_surfaceview);
        btStart = (Button) findViewById(R.id.bt_start);
    }

    @SuppressLint("ClickableViewAccessibility")
    protected void init() {
        super.init();
        btStart.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        btStart.setText("请说话");
                        flag = true;
                        FFmpegUtils.setFlag(flag);
                        break;
                    case MotionEvent.ACTION_UP:
                        btStart.setText("按住说话");
                        flag = false;
                        FFmpegUtils.setFlag(flag);
                        break;
                }
                return false;
            }
        });
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                channel, pcmFormat,
                pcmSize);

        bytes = new byte[pcmSize];
    }

    private AudioRead audioReadThread;

    private void startAudioRead() {
        stopAudioRead();
        audioRecord.startRecording();
        if (audioReadThread == null) {
            audioReadThread = new AudioRead();
            audioReadThread.readFlag = true;
            audioReadThread.start();
        }
    }

    private void stopAudioRead() {
        audioRecord.stop();
        if (audioReadThread != null) {
            audioReadThread.readFlag = false;
            try {
                audioReadThread.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        audioReadThread = null;
    }

    class AudioRead extends Thread {

        int readSize = 0;
        boolean readFlag;

        @Override
        public void run() {
            super.run();
            while (readFlag) {
                if (flag) {
                    readSize = audioRecord.read(bytes, 0, pcmSize);
                    Log.e("xhc" , " read size "+readSize);
                    if (AudioRecord.ERROR_INVALID_OPERATION != readSize) {
                        FFmpegUtils.videoDubAddVoice(bytes);
                    }
                }
            }
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            myVideoGpuShow.setPlayPath(listPath.get(0));
            startAudioRead();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopAudioRead();
        FFmpegUtils.videoDubDestroy();
    }
}
