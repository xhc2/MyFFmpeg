package module.video.jnc.myffmpeg.activity;

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

import java.util.List;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.Constant;
import module.video.jnc.myffmpeg.widget.CameraPreview;

public class CameraStreamActivity extends Activity implements Camera.PreviewCallback {

    private Camera mCamera;
    private CameraPreview mPreview;
    private FrameLayout preview;
    private Camera.Parameters params;
    private String ouputPath;
    //默认前置 记录当前的方向
    private int nowCameraDirection = Camera.CameraInfo.CAMERA_FACING_BACK;
    private int height, width;

    private TextView tv;
    private AudioRecord audioRecord;

    private static final int sampleRate = 44100;
    private static final int pcmFormat = AudioFormat.ENCODING_PCM_16BIT;
    private static final int channel = AudioFormat.CHANNEL_IN_MONO;

    private AudioRead audioRead;
    private boolean audioReadFlag = false;
    private byte[] bytes;
    private int pcmSize;
    private boolean isRecord = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_stream);
        ouputPath = "rtmp://192.168.15.239:1935/live/live";//"sdcard/FFmpeg/camerastream.flv"; //getIntent().getStringExtra("outputpath");
        preview = (FrameLayout) findViewById(R.id.camera_preview);
        tv = (TextView) findViewById(R.id.bt_record);
//        initView();
        pcmSize = 4096;
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                channel, pcmFormat,
                pcmSize);
        bytes = new byte[pcmSize];
        findViewById(R.id.bt_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!isRecord) {
                    startPublish();
                    startAudioRead();
                    isRecord = !isRecord;
                    tv.setText("停止");
                } else {
                    finish();
                }
            }
        });
    }

    private void startAudioRead() {
        stopAudioRead();
        audioRecord.startRecording();
        audioReadFlag = true;
        if (audioRead == null) {
            audioRead = new AudioRead();
            audioRead.start();
        }
    }

    private void stopAudioRead() {
        audioReadFlag = false;
        audioRecord.stop();
        if (audioRead != null) {
            try {
                audioRead.join();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        audioRead = null;
    }

    class AudioRead extends Thread {

        int readSize = 0;

        @Override
        public void run() {
            super.run();
            while (audioReadFlag) {
                readSize = audioRecord.read(bytes, 0, pcmSize);
                if (AudioRecord.ERROR_INVALID_OPERATION != readSize) {
                    FFmpegUtils.rtmpAudioStream(bytes, readSize);
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
        for (Camera.Size s : list) {
            if (s.height < 500 && s.height > 300) {
                Log.e("xhc", "width " + s.width + " height " + s.height);
                width = s.width;
                height = s.height;
                break;
            }
        }

        params.setPreviewSize(width, height);
        params.setPreviewFormat(ImageFormat.YV12);
        mCamera.setParameters(params);

        mPreview = new CameraPreview(this, mCamera, this);
        if (preview.getChildCount() > 0) {
            preview.removeAllViews();
        }
        preview.addView(mPreview);

        if (params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
            params.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
        }

    }

    private void startPublish() {
        stopPublish();
        if (publishThread == null) {
            publishThread = new PublishThread();
            publishThread.start();
        }
    }

    private void stopPublish() {
        FFmpegUtils.rtmpDestroy();
        if (publishThread != null) {
            try {
                publishThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        publishThread = null;
    }


    private PublishThread publishThread;

    class PublishThread extends Thread {
        @Override
        public void run() {
            super.run();
            FFmpegUtils.rtmpCameraInit(ouputPath, width, height, pcmSize);
            FFmpegUtils.startRecord();
        }
    }

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
        FFmpegUtils.rtmpCameraStream(data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        releaseCamera();
    }

    private void releaseCamera() {
        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.release();        // release the camera for other applications
            mCamera = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopAudioRead();
        stopPublish();
    }
}
