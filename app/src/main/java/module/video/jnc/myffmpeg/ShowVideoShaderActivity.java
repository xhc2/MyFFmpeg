package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

public class ShowVideoShaderActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_video_shader);
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.e("xhc" ," onPause ");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.e("xhc" ," ondestroy ");
        FFmpegUtils.showVideoGpuDestroy();
    }
}
