package module.video.jnc.myffmpeg;

import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

public class MyRecordActivity extends AppCompatActivity {

    private FrameLayout fl ;
    CameraManeger cm;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_record);
        FFmpegUtils.myInit(Constant.rootFile.getAbsolutePath()+"/my_camera.MP4" , CameraManeger.width  , CameraManeger.height);
        fl = findViewById(R.id.container);
        cm = new CameraManeger();
        final Camera camera = cm.OpenCamera();
        CameraPreview cp = new CameraPreview(this , camera);
        fl.addView(cp);

        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
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



    @Override
    protected void onStop() {
        super.onStop();
        cm.closeCamera();
        FFmpegUtils.closeMyFFmpeg();
    }
}
