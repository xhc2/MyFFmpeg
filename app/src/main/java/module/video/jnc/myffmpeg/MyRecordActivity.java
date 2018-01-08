package module.video.jnc.myffmpeg;

import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

public class MyRecordActivity extends AppCompatActivity {

    private SurfaceView surfaceView;
    private FrameLayout fl ;
    CameraManeger cm;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_record);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        Camera camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this , camera);
        fl.addView(cp);
        camera.setPreviewCallback(new Camera.PreviewCallback(){
            @Override
            public void onPreviewFrame(byte[] bytes, Camera camera) {
                FFmpegUtils.encodeCamera(bytes);
            }
        });

        FFmpegUtils.myInit(Constant.rootFile.getAbsolutePath()+"/my_camera.MP4" , CameraManeger.width  , CameraManeger.height);

    }

    @Override
    protected void onStop() {
        super.onStop();
        cm.closeCamera();
        FFmpegUtils.closeMyFFmpeg();

    }
}
