package module.video.jnc.myffmpeg;

import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.util.List;

public class CameraStreamActivity extends Activity implements  Camera.PreviewCallback{

    private Camera mCamera;
    private CameraPreview mPreview;
    private FrameLayout preview;
    private Camera.Parameters params;
    private byte[] buffer ;
    //默认前置 记录当前的方向
    private int nowCameraDirection = Camera.CameraInfo.CAMERA_FACING_FRONT;
    private int height , width ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_stream);
        preview = (FrameLayout)findViewById(R.id.camera_preview);

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

            if(s.height < 400 && s.height > 300){
                Log.e("xhc" , "width "+s.width+" height "+s.height);
                width = s.width;
                height = s.height;
                break;
            }
        }

        params.setPreviewSize(width, height );
        FFmpegUtils.rtmpCameraInit("rtmp://192.168.2.109/live/live" ,width  , height);
//        buffer = new byte[(int)(width * height * 1.5f)];
        params.setPreviewFormat(ImageFormat.NV21);
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
//        System.arraycopy(data , 0 , buffer , 0 , data.length);
        FFmpegUtils.rtmpCameraStream(data);
    }
}
