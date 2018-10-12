package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

import module.video.jnc.myffmpeg.MediaCodec.MediaCodecAudioEncoder;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecVideoEncoder;
import module.video.jnc.myffmpeg.MediaCodec.MyMediaMuxer;

public class HardMuxerCameraActivity extends Activity implements Camera.PreviewCallback, MediaCodecVideoEncoder.H264CallBack, MediaCodecAudioEncoder.AACCallBack {

    private Camera mCamera;
    private CameraPreview mPreview;
    private FrameLayout preview;
    private Camera.Parameters params;
    private String ouputPath;
    //默认前置 记录当前的方向
    private int nowCameraDirection = Camera.CameraInfo.CAMERA_FACING_BACK;
    private int height, width;
    private boolean isRecord = false;
    private TextView tv;
    private AudioRecord audioRecord;

    private static final int sampleRate = 44100;
    private static final int pcmFormat = AudioFormat.ENCODING_PCM_16BIT;
    private static final int channel = AudioFormat.CHANNEL_IN_MONO;
    private static final int channelCount = 2;

    private AudioRead audioRead;
    private boolean audioReadFlag = false;
    private byte[] bytes;
    private int pcmSize;

    private MediaCodecVideoEncoder vEncoder;
    private MediaCodecAudioEncoder aEncoder;
    private MyMediaMuxer muxer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hard_muxer_camera);
        ouputPath = "sdcard/FFmpeg/hard_muxer.mp4";
        preview = (FrameLayout) findViewById(R.id.camera_preview);
        tv = (TextView) findViewById(R.id.bt_record);
        pcmSize = 4096;//AudioRecord.getMinBufferSize(sampleRate , channel , pcmFormat);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate,
                channel, pcmFormat,
                pcmSize);
        bytes = new byte[pcmSize];
        audioRecord.startRecording();
        startAudioRead();
        findViewById(R.id.bt_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                isRecord = !isRecord;
                if (isRecord) {
                    tv.setText("暂停");
                } else {
                    tv.setText("播放");
                }
            }
        });
    }

    private void initMedia() {
        muxer = new MyMediaMuxer(ouputPath);

        vEncoder = new MediaCodecVideoEncoder(width, height, this);
        vEncoder.addTrack(new MediaCodecVideoEncoder.AddTrackInter() {
            @Override
            public void addTrack(MediaFormat format) {
                muxer.addVideoTrack(format);
                muxer.startMuxer();
            }
        });
        vEncoder.startEncode();

        aEncoder = new MediaCodecAudioEncoder(sampleRate, channelCount, pcmSize, this);
        aEncoder.addTrack(new MediaCodecAudioEncoder.AddTrackInter() {
            @Override
            public void addTrack(MediaFormat format) {
                muxer.addAudioTrack(format);
                muxer.startMuxer();
            }
        });
        aEncoder.addHeadFlag(true);
        aEncoder.startEncode();
    }

    private void startAudioRead() {
        stopAudioRead();
        audioReadFlag = true;
        if (audioRead == null) {
            audioRead = new AudioRead();
            audioRead.start();
        }
    }

    private void stopAudioRead() {
        audioReadFlag = false;
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
                if (isRecord) {
                    readSize = audioRecord.read(bytes, 0, pcmSize);

                    if (AudioRecord.ERROR_INVALID_OPERATION != readSize) {
                        if (readSize != bytes.length) {
                            byte[] tempbuffer = new byte[readSize];
                            System.arraycopy(bytes, 0, tempbuffer, 0, readSize);
                            aEncoder.addData(tempbuffer);
                        } else {
                            aEncoder.addData(bytes);
                        }

                    }
                }
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        initView();
        initMedia();
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
                Log.e("xhc", " width " + s.width + " height " + s.height);
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
        if (isRecord) {
            vEncoder.addByte(data);
        }
    }




    @Override
    public void H264CallBack(byte[] data, MediaCodec.BufferInfo info, ByteBuffer buffer) {
        //h264CallBack
        if (data != null) {
            Log.e("xhc", " data " + data.length);
            muxer.writeVideoData(buffer , info);
        }
    }


    @Override
    public void aacCallBack(byte[] buffer, MediaCodec.BufferInfo info, ByteBuffer byteBuffer) {
        if (buffer != null) {
            muxer.writeAudioData(byteBuffer , info);
        }
    }
    private void releaseCamera(){
        if (mCamera != null){
            mCamera.release();        // release the camera for other applications
            mCamera = null;
        }
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopAudioRead();
        if(aEncoder != null){
            aEncoder.stopEncode();
        }
        if(vEncoder != null){
            vEncoder.stopEncode();
        }
        if(muxer !=  null){
            muxer.stopMuxer();
        }
        releaseCamera();
    }

}
