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

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

public class MyRecordActivity extends AppCompatActivity {

    private FrameLayout fl ;
    private CameraManeger cm;
    private AudioRecord ar ;
    private int size;
    private boolean recordFlag = false;
    Camera camera;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_record);
        FFmpegUtils.myInit(Constant.rootFile.getAbsolutePath()+"/my_camera.MP4" , CameraManeger.width  , CameraManeger.height);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        size = AudioRecord.getMinBufferSize(44100 , AudioFormat. CHANNEL_IN_MONO  , AudioFormat.ENCODING_PCM_8BIT );
        Log.e("xhc" , " min size "+size );
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC , 44100 , AudioFormat.CHANNEL_IN_MONO ,AudioFormat.ENCODING_PCM_8BIT , size );
        camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this , camera);
        fl.addView(cp);
        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                camera.setPreviewCallback(new Camera.PreviewCallback(){
                    @Override
                    public void onPreviewFrame(byte[] bytes, Camera camera) {

                        FFmpegUtils.nv21ToYv12(bytes);
                        FFmpegUtils.encodeCamera(bytes);
                        startReocrdAudio();
                        findViewById(R.id.bt_start).setEnabled(false);
                    }
                });
            }
        });
    }

    private void startReocrdAudio(){
        ar.startRecording();
        new RecordThread().start();
        recordFlag = true;
    }

    private void releaseAudioRecord(){
        if(ar != null){
            ar.stop();
            ar.release();
            ar = null;
        }
    }


    class RecordThread extends Thread{

        @Override
        public void run() {
            super.run();
            byte[] buffer = new byte[size];
            while(recordFlag){
                Log.e("xhc" , " audiorecord ...");
                ar.read(buffer , 0 , buffer.length);
                FFmpegUtils.encodePcm(buffer , buffer.length);
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
