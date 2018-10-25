package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.os.Bundle;
import java.util.ArrayList;
import java.util.List;
import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;


public class VideoClipActivity extends VideoEditParentActivity   {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_deal_video);
        titleBar = findViewById(R.id.title);
        if (listPath.size() > 0) {
            //这里播放视频。我的mp4的播放器部分需要修改opengl部分

        }
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                showDialog("loading");
            }
        });
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.destroyClip();
    }
}
