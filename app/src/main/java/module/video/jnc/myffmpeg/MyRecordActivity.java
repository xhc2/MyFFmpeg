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

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

public class MyRecordActivity extends AppCompatActivity {

    private FrameLayout fl ;
    private CameraManeger cm;
    private AudioRecord ar ;
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
        FFmpegUtils.myInit(Constant.rootFile.getAbsolutePath()+"/my_camera.MP4" , CameraManeger.width  , CameraManeger.height);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        size = AudioRecord.getMinBufferSize(frequency , channelConfiguration  , EncodingBitRate );
        Log.e("xhc" , " min size "+size );
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC , frequency , channelConfiguration ,EncodingBitRate , size );
        camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this , camera);
        fl.addView(cp);


        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startReocrdAudio();
                camera.setPreviewCallback(new Camera.PreviewCallback(){
                    @Override
                    public void onPreviewFrame(byte[] bytes, Camera camera) {

                        FFmpegUtils.nv21ToYv12(bytes);
                        FFmpegUtils.encodeCamera(bytes);
                        findViewById(R.id.bt_start).setEnabled(false);
                    }
                });
            }
        });

    }

    private void startReocrdAudio(){
        if(ar.getState() == AudioRecord.STATE_INITIALIZED){
            Log.e("xhc" , "AudioRecord.STATE_INITIALIZED ");
        }
        else{
            Log.e("xhc" , "AudioRecord.STATE_unINITIALIZED ");
        }
        ar.startRecording();
        new RecordThread().start();
        recordFlag = true;
    }

    private void releaseAudioRecord(){
        if(ar != null){
            recordFlag = false;
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

            FileOutputStream fos = null;
            try{
                File file = new File("sdcard/FFmpeg/my_audio2.pcm");
                fos = new FileOutputStream(file , true);
            }catch (Exception e){

            }
            int read = 0 ;
            while(recordFlag){
                read  = ar.read(buffer , 0 , buffer.length);
                Log.e("xhc" , "read "+read );
                if(AudioRecord.ERROR_INVALID_OPERATION != read){
                    try{
                        fos.write(buffer , 0 , read);
                    }catch(Exception e){
                        Log.e("xhc" , " excpeiont "+e.getMessage());
                        e.printStackTrace();
                    }
                }
//                FFmpegUtils.encodePcm(buffer , buffer.length);
            }
            try{
                fos.flush();
                fos.close();
            }catch (Exception e){
                e.printStackTrace();
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
