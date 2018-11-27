package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoEditParentActivity extends BaseActivity {
    protected List<String> listPath = new ArrayList<>();
    protected TitleBar titleBar;
    protected MyVideoGpuShow myVideoGpuShow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        ArrayList<String> tempVideos = intent.getStringArrayListExtra("videos");
        if (tempVideos != null) {
            listPath.addAll(tempVideos);
        }


    }
}
