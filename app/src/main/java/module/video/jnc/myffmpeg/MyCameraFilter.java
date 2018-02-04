package module.video.jnc.myffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MyCameraFilter extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_camera_filter);

        FFmpegUtils.filterCameraInit(Constant.rootFile+"/"+"my_camera_filter.mp4");
    }
}
