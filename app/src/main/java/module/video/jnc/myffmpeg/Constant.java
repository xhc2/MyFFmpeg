package module.video.jnc.myffmpeg;

import android.content.Context;
import android.hardware.Camera;
import android.os.Environment;
import android.widget.Toast;

import java.io.File;

/**
 * Created by xhc on 2017/11/21.
 */

public class Constant {
    /** A safe way to get an instance of the Camera object. */
    public static Camera getCameraInstance(Context context , int direction){
        Camera c = null;
        try {
            c = Camera.open(direction); // attempt to get a Camera instance
            if(direction == Camera.CameraInfo.CAMERA_FACING_FRONT){
                c.setDisplayOrientation(90);
            }
            else{
                c.setDisplayOrientation(90);
            }

        }
        catch (Exception e){
            // Camera is not available (in use or does not exist)
            Toast.makeText(context,"你的相机不可用",Toast.LENGTH_SHORT).show();
        }
        return c; // returns null if camera is unavailable
    }

    public static File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
    public static File rootVideoFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/video");
}
