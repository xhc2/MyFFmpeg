package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;

/**
 * Created by xhc on 2018/1/5.
 */

public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    private SurfaceHolder mHolder;
    private Camera mCamera;
    private Camera.Parameters params;
    private Context context;
    private Camera.PreviewCallback callback;
    public CameraPreview(Context context, Camera camera  ,Camera.PreviewCallback callback) {
        super(context);
        Log.e("xhc", "CameraPreview constron");
        mCamera = camera;
        this.context = context;
        params = mCamera.getParameters();
        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);
        this.callback = callback;
        // deprecated setting, but required on Android versions prior to 3.0
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }
//
    public void surfaceCreated(SurfaceHolder holder) {
        Log.e("xhc", "surfaceCreated");
        // The Surface has been created, now tell the camera where to draw the preview.
        try {
            mCamera.setPreviewDisplay(holder);
            if(this.callback != null){
                mCamera.setPreviewCallback(this.callback);
            }
            mCamera.startPreview();
        } catch (IOException e) {
            Log.d("asdf", "Error setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.e("xhc", "surfaceDestroyed");
        // empty. Take care of releasing the Camera preview in your activity.
        try {
            mCamera.stopPreview();
        } catch (Exception e) {
            // ignore: tried to stop a non-existent preview
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.e("xhc", "surfaceChanged");
        // If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.

        if (mHolder.getSurface() == null) {
            // preview surface does not exist
            return;
        }

        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e) {
            // ignore: tried to stop a non-existent preview
        }

        // set preview size and make any resize, rotate or
        // reformatting changes here

        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(mHolder);
            if(this.callback != null){
                mCamera.setPreviewCallback(this.callback);
            }
            mCamera.startPreview();
        } catch (Exception e) {
        }

        autoFocus();
    }


    private void autoFocus() {
        if (params.getSupportedFocusModes().contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
            mCamera.autoFocus(new Camera.AutoFocusCallback() {
                @Override
                public void onAutoFocus(boolean success, Camera camera) {
                    if (success) {
                    }
                }

            });
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            autoFocus();
        }
        return super.onTouchEvent(event);
    }
}