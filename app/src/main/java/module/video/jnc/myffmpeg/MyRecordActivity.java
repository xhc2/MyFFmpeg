package module.video.jnc.myffmpeg;

import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.AudioRouting;
import android.media.MediaRecorder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;
/*
  只是将yuv数据压缩进去
 */
public class MyRecordActivity extends AppCompatActivity {

    private FrameLayout fl;
    private CameraManeger cm;
    private AudioRecord ar;
    private int size;
    private boolean recordFlag = false;
    Camera camera;

    private static int frequency = 44100;

    private static int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;//单声道

    private static int EncodingBitRate = AudioFormat.ENCODING_PCM_16BIT;    //音频数据格式：脉冲编码调制（PCM）每个样品16位

    private static List<byte[]> list = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_record);
        FFmpegUtils.myInit(Constant.rootFile.getAbsolutePath() + "/my_camera.MP4", CameraManeger.width, CameraManeger.height);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        size = AudioRecord.getMinBufferSize(frequency, channelConfiguration, EncodingBitRate);
        Log.e("xhc", " min size " + size);
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency, channelConfiguration, EncodingBitRate, size);
        camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this, camera);
        fl.addView(cp);


        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startWriteBufferThread();
                camera.setPreviewCallback(new Camera.PreviewCallback() {
                    @Override
                    public void onPreviewFrame(byte[] bytes, Camera camera) {
                        list.add(bytes);
                        findViewById(R.id.bt_start).setEnabled(false);
                    }
                });
            }
        });
    }

    private class WriteBufferThread extends Thread{

        public boolean flag = false;

        @Override
        public void run() {
            super.run();

            while (flag){
                if(!list.isEmpty()){
                    byte[] bytes = list.remove(0);
                    FFmpegUtils.nv21ToYv12(bytes);
                    FFmpegUtils.encodeCamera(bytes);
                }
            }
        }
    }

    WriteBufferThread wbt ;

    private void startWriteBufferThread(){
        wbt = new WriteBufferThread();
        wbt.flag = true;
        wbt.start();
    }
    private void stopWriteBufferThread(){
        if(wbt != null){
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
                    FFmpegUtils.encodePcm(buffer, buffer.length);
                }
            }
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        recordFlag = false;
        camera.setPreviewCallback(null);
        cm.closeCamera();
        FFmpegUtils.closeMyFFmpeg();
        releaseAudioRecord();
    }
}
