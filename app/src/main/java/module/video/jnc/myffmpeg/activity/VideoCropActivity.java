package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoCropActivity extends VideoEditParentActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_crop);
        findViewById();
        init();
    }
    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
    }

    protected void init() {
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
                FileUtils.makeCropDir();
                FFmpegUtils.initVideoFilter(listPath.get(0) , FileUtils.APP_CROP + System.currentTimeMillis()+".mp4" ,
                        "crop=100:100" , new int[]{100 , 100});
                FFmpegUtils.videoFilterStart();
//                if (dealFlag) {
//                    showToast("正在处理中，请稍等");
//                    showLoadPorgressDialog("处理中...");
//                    return;
//                }
//                startWaterMarkThread();
//                startProgressThread();
//                showLoadPorgressDialog("处理中...");
            }
        });
    }


    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
            startPlayThread(listPath.get(0));
        }
    }
}
