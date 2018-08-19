package module.video.jnc.myffmpeg;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.util.List;

public class CameraStreamActivity extends Activity implements  Camera.PreviewCallback{

    private Camera mCamera;
    private CameraPreview mPreview;
    private FrameLayout preview;
    private Camera.Parameters params;
//    private String ouputPath = "rtmp://192.168.2.15/live/live";
    private String ouputPath = "sdcard/FFmpeg/cameraStream.flv";
    //默认前置 记录当前的方向
    private int nowCameraDirection = Camera.CameraInfo.CAMERA_FACING_BACK;
    private int height , width ;
    private boolean isRecord = false;
    private TextView tv ;
    private AudioRecord audioRecord;
    private static final int sampleRate = 44100;
    private static final int pcmFormat = AudioFormat.ENCODING_PCM_16BIT;
    private static final int channel = AudioFormat.CHANNEL_IN_MONO;
    private AudioRead audioRead;
    private boolean audioReadFlag = false;
    private byte[] bytes ;
    private int pcmSize ;
//    private File file ;
//    private FileOutputStream fos ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_stream);
        preview = (FrameLayout)findViewById(R.id.camera_preview);
        tv = (TextView)findViewById(R.id.bt_record);
        pcmSize = 4096;//AudioRecord.getMinBufferSize(sampleRate , channel , pcmFormat);
        Log.e("xhc_jni" , " pcm size "+pcmSize);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC , sampleRate ,
                channel , pcmFormat,
                pcmSize);
        bytes = new byte[pcmSize];
        audioRecord.startRecording();
        startAudioRead();
        findViewById(R.id.bt_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isRecord = !isRecord;
                if(isRecord){
                    tv.setText("暂停");
                    FFmpegUtils.startRecord();
                }
                else{
                    tv.setText("播放");
                    FFmpegUtils.pauseRecord();
                }
            }
        });
    }

    private void startAudioRead(){
        stopAudioRead();
        audioReadFlag = true;
        if(audioRead == null){
            audioRead = new AudioRead();
            audioRead.start();
        }
    }

    private void stopAudioRead(){
        audioReadFlag = false;
        if(audioRead != null){
            try{
                audioRead.join();
            }
            catch (Exception e){
                e.printStackTrace();
            }
        }
        audioRead = null;
    }

    class AudioRead extends Thread{

        int readSize = 0;

        @Override
        public void run() {
            super.run();
            while(audioReadFlag){
                if(isRecord){
                    readSize = audioRecord.read(bytes , 0 , pcmSize);
                    if(AudioRecord.ERROR_INVALID_OPERATION != readSize ){
                        FFmpegUtils.rtmpAudioStream(bytes ,readSize );
                    }
                }
            }
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        initView();
    }

    private void initView() {
        if (!checkCameraHardware(this)) {
            Toast.makeText(this, "手机不支持相机", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        if (!this.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA_FRONT)) {
            Toast.makeText(this, "手机不支持前置摄像", Toast.LENGTH_SHORT).show();
            nowCameraDirection = Camera.CameraInfo.CAMERA_FACING_BACK;
        }
        // Create an instance of Camera
        mCamera = Constant.getCameraInstance(this, nowCameraDirection);
        params = mCamera.getParameters();
        params.set("orientation", "portrait");
        List<Camera.Size> list = params.getSupportedPreviewSizes();
        for(Camera.Size s : list){
            if(s.height < 500 && s.height > 300){
                Log.e("xhc" , "width "+s.width+" height "+s.height);
                width = s.width;
                height = s.height;
                break;
            }
        }

        params.setPreviewSize(width, height );
        FFmpegUtils.rtmpCameraInit(ouputPath ,width  , height  , pcmSize);
        params.setPreviewFormat(ImageFormat.YV12);
        mCamera.setParameters(params);

        mPreview = new CameraPreview(this, mCamera ,this);
        if(preview.getChildCount()>0){
            preview.removeAllViews();
        }
        preview.addView(mPreview);

        if (params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
            params.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
        }
    }

    /**
     * Check if this device has a camera
     */
    private boolean checkCameraHardware(Context context) {
        if (context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)) {
            // this device has a camera
            return true;
        } else {
            // no camera on this device
            return false;
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        if(isRecord){
//            FFmpegUtils.rtmpCameraStream(data);
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.rtmpDestroy();
        stopAudioRead();
    }
}
