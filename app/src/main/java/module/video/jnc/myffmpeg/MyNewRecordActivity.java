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

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

/**
 * 将视频数据和声音数据同时加入进去
 */
public class MyNewRecordActivity extends AppCompatActivity {

    private FrameLayout fl;
    private CameraManeger cm;
    private AudioRecord ar;
    private int size;
    private boolean recordFlag = false;
    Camera camera;

    private static int frequency = 44100;

    private static int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;//单声道

    private static int EncodingBitRate = AudioFormat.ENCODING_PCM_16BIT;    //音频数据格式：脉冲编码调制（PCM）每个样品16位

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_record);

        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        size = AudioRecord.getMinBufferSize(frequency, channelConfiguration, EncodingBitRate);
        Log.e("xhc", " min size " + size);
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency, channelConfiguration, EncodingBitRate, size);
        camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this, camera);
        fl.addView(cp);
        FFmpegUtils.initMyCameraMuxer(Constant.rootFile.getAbsolutePath() + "/my_new_camera.MP4", CameraManeger.width, CameraManeger.height , size);

        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startReocrdAudio();
                camera.setPreviewCallback(new Camera.PreviewCallback() {
                    @Override
                    public void onPreviewFrame(byte[] bytes, Camera camera) {

//                        FFmpegUtils.nv21ToYv12(bytes);
                        FFmpegUtils.encodeMyMuxerCamera(bytes);
                        findViewById(R.id.bt_start).setEnabled(false);
                    }
                });
            }
        });

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
//                    FFmpegUtils.encodePcm(buffer, buffer.length);
                    FFmpegUtils.encodeMyMuxerAudio(buffer);
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
        FFmpegUtils.closeMyMuxer();
        releaseAudioRecord();
    }
}
