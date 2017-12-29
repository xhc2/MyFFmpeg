package module.video.jnc.myffmpeg.EGLCamera;

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

    public void OpenCamera(SurfaceTexture surfaceTexture) {
        try {
            mCamera = Camera.open(CAMERA_FACING_FRONT);
            Camera.Parameters params = mCamera.getParameters();
//            params.setPreviewSize(1080 , 1920);
            params.setPreviewSize(1280, 720 );
            List<Camera.Size> list = params.getSupportedPreviewSizes();
            for(Camera.Size s:list){
                Log.e("xhc" , "width "+s.width+" height "+s.height);
            }
            mCamera.setParameters(params);
            mCamera.setPreviewTexture(surfaceTexture);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.e("xhc" ," camera exception "+e.getMessage());
            e.printStackTrace();
        }
    }

}
