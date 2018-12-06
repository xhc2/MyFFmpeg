package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.ClipBar;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class Mp42GifActivity extends VideoEditParentActivity  implements ClipBar.TouchCallBack {
    private ClipBar clipBar;
    private TextView tvStart;
    private TextView tvEnd;
    private int startTime = -1;
    private int endTime = -1;
    private ImageView img;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mp42_gif);
        findViewById();
        init();
    }

    private void findViewById() {
        clipBar = findViewById(R.id.clip_bar);
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        tvStart = findViewById(R.id.tv_start);
        tvEnd = findViewById(R.id.tv_end);
        img = findViewById(R.id.img);
    }

    protected void init() {
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                FileUtils.makeGifDir();
                if (dealFlag) {
                    showToast("正在裁剪中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
//                if (startTime == -1 || endTime == -1 || startTime >= endTime || startTime < 0 || endTime > FFmpegUtils.getDuration()) {
//                    showToast("裁剪时间有问题！");
//                    return;
//                }
                showLoadPorgressDialog("处理中...");
                FFmpegUtils.initGif(listPath.get(0) , FileUtils.APP_GIF+"gif_"+System.currentTimeMillis()+".gif" );
                FFmpegUtils.startGifParse();
            }
        });
        clipBar.setTouchCallBack(this);

    }

    @Override
    public void moveStart(float screenStartX, int startProgress) {
        float startTime = ((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration();
        tvStart.setText("开始时间： " + startTime);
//        es.execute(new CurrentBitmapThread(startTime));
//        img.setX(screenStartX);
//        img.setImageBitmap(BitmapFactory.decodeByteArray(buffer, 0, buffer.length));
    }

    @Override
    public void moveEnd(float screenEndX, int endProgress) {
        float endTime = ((float) endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration();
        tvEnd.setText("结束时间： " + endTime);
    }

    @Override
    public void moveFinish(int startProgress, int endProgress) {
        Log.e("xhc", " startProgress " + startProgress + " endProgress " + endProgress);
        startTime = (int) (((float) startProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        endTime = (int) (((float) endProgress / clipBar.getMaxProgress()) * FFmpegUtils.getDuration());
        tvStart.setText("开始时间： " + startTime);
        tvEnd.setText("结束时间： " + endTime);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
            activityFoucsFlag = true;
//            startPlayThread(listPath.get(0));
        }
    }

}
