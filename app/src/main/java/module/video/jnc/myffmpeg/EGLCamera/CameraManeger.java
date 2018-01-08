package module.video.jnc.myffmpeg.EGLCamera;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import java.io.IOException;
import java.util.List;

import static android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK;
import static android.hardware.Camera.CameraInfo.CAMERA_FACING_FRONT;

/**
 * Created by Administrator on 2017-07-13.
 */

public class CameraManeger {

    private Camera mCamera;
    public final static  int width = 640;
    public final static int height = 480;
    public void OpenCamera(SurfaceTexture surfaceTexture) {
        try {
            mCamera = Camera.open(CAMERA_FACING_FRONT);
            Camera.Parameters params = mCamera.getParameters();
//            params.setPreviewSize(1920 , 1080);
            params.setPreviewSize(width, height );
            List<Camera.Size> list = params.getSupportedPreviewSizes();
            for(Camera.Size s:list){
                Log.e("xhc" , "width "+s.width+" height "+s.height);
            }
            params.setPreviewFormat(ImageFormat.YV12);
            mCamera.setParameters(params);
            mCamera.setPreviewTexture(surfaceTexture);

            mCamera.startPreview();
        } catch (IOException e) {
            Log.e("xhc" ," camera exception "+e.getMessage());
            e.printStackTrace();
        }
    }

    public Camera OpenCamera(){
        mCamera = Camera.open(CAMERA_FACING_FRONT);
        mCamera.setDisplayOrientation(90);
        Camera.Parameters params = mCamera.getParameters();
        params.setPreviewSize(width, height );
        mCamera.setParameters(params);
        return mCamera;
    }

    public void closeCamera(){
        if(mCamera != null){
            mCamera.stopPreview();
            mCamera.release();
        }
    }

}
