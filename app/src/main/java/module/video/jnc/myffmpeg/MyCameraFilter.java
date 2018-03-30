package module.video.jnc.myffmpeg;

import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import java.util.LinkedList;
import java.util.List;

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

/**
 * 通过摄像头，和音频的录制。
 * 然后利用ffmpeg来编码和混合成一个视频
 */
public class MyCameraFilter extends AppCompatActivity {
    private FrameLayout fl;
    private CameraManeger cm;
    private AudioRecord ar;
    private boolean recordFlag = false;
    private Camera camera;
    private int size = 2048;
    private static int frequency = 44100;

    private static int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;//单声道

    private static int EncodingBitRate = AudioFormat.ENCODING_PCM_16BIT;    //音频数据格式：脉冲编码调制（PCM）每个样品16位

    private static List<byte[]> listVideo = new LinkedList<>();
    private static List<byte[]> listAudio = new LinkedList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_camera_filter);
        FFmpegUtils.filterCameraInit(Constant.rootFile + "/" + "my_camera_filter.mp4", CameraManeger.width, CameraManeger.height, size);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency, channelConfiguration, EncodingBitRate, size);
        camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this, camera);
        fl.addView(cp);

        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                startReocrdAudio();
                startWriteBufferThread();
                startDealAudio();
                camera.setPreviewCallback(new Camera.PreviewCallback() {
                    @Override
                    public void onPreviewFrame(byte[] bytes, Camera camera) {
                        listVideo.add(bytes);
                    }
                });
                findViewById(R.id.bt_start).setEnabled(false);
            }
        });
    }

    private class WriteBufferThread extends Thread {

        public boolean flag = false;

        @Override
        public void run() {
            super.run();

            while (flag) {
                if (!listVideo.isEmpty()) {
                    byte[] bytes = listVideo.get(0);
                    if (FFmpegUtils.encodeMyMuxerCameraFilter(bytes) > 0) {
                        listVideo.remove(0);
                    }
                }
            }
        }
    }

    WriteBufferThread wbt;

    private void startWriteBufferThread() {
        wbt = new WriteBufferThread();
        wbt.flag = true;
        wbt.start();
    }

    private void stopWriteBufferThread() {
        if (wbt != null) {
            wbt.flag = false;
        }
    }


    private void startReocrdAudio() {
        if (ar.getState() == AudioRecord.STATE_INITIALIZED) {
            Log.e("xhc", "AudioRecord.STATE_INITIALIZED ");
        } else {
            Log.e("xhc", "AudioRecord.STATE_unINITIALIZED ");
        }
        ar.startRecording();
        new RecordThread().start();
        recordFlag = true;
    }

    private void releaseAudioRecord() {
        if (ar != null) {
            recordFlag = false;
            ar.stop();
            ar.release();
            ar = null;
        }
    }


    class RecordThread extends Thread {

        @Override
        public void run() {
            super.run();
            byte[] buffer = new byte[size];
            int read = 0;
            while (recordFlag) {
                read = ar.read(buffer, 0, buffer.length);
                Log.e("xhc", "read " + read);
                if (AudioRecord.ERROR_INVALID_OPERATION != read) {
                    listAudio.add(buffer);
                }
            }
        }
    }

    class DealAudioThread extends Thread {
        boolean flag = false;

        @Override
        public void run() {
            super.run();
            while (flag) {
                if (!listAudio.isEmpty()) {
                    if (FFmpegUtils.encodeMyMuxerAudioFilter(listAudio.get(0)) > 0) {
                        listAudio.remove(0);
                    }
                }
            }
        }
    }


    DealAudioThread daThread;

    private void startDealAudio() {
        daThread = new DealAudioThread();
        daThread.flag = true;
        daThread.start();
    }

    private void stopDealAudio() {
        if (daThread != null) {
            daThread.flag = false;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        recordFlag = false;
        camera.setPreviewCallback(null);
        cm.closeCamera();
        FFmpegUtils.closeMyMuxerFilter();
        releaseAudioRecord();
        stopWriteBufferThread();
        stopDealAudio();
    }
}
