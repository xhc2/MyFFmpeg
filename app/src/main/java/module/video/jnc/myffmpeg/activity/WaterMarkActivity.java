package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyRender;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class WaterMarkActivity extends VideoEditParentActivity {

    private ImageView img ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_water_mark);
        findViewById();
        init();
    }
    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        img = findViewById(R.id.img);
    }
    protected void init(){
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击
//                if (clipFlag) {
////                    showToast("正在裁剪中，请稍等");
//                    showLoadPorgressDialog("处理中...");
//                    return;
//                }
//                if (startTime == -1 || endTime == -1 || startTime >= endTime || startTime < 0 || endTime > FFmpegUtils.getDuration()) {
//                    showToast("裁剪时间有问题！");
//                    return;
//                }
//                showLoadPorgressDialog("处理中...");
//                startClip(startTime, endTime);
//                startProgressThread();
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
