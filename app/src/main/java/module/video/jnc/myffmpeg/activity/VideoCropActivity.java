package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import java.util.Locale;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.CropGraph;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoCropActivity extends SingleFilterActivity     {

    private CropGraph cgGraph;
    private int videoWidthPx;
    private int videoHeightPx;

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
        cgGraph = (CropGraph)findViewById(R.id.cg_crop_area);
    }

    protected void init() {
        super.init();
        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
            @Override
            public void clickRight() {
                //右键点击

                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
                FileUtils.makeCropDir();
                int[] result = cgGraph.getGraphResult();
                int startX = (int)((float)videoWidthPx /  myVideoGpuShow.getWidth() * result[0] );
                int startY = (int)((float)videoHeightPx /  myVideoGpuShow.getHeight() * result[1]) ;
                int width = (int)((float)videoWidthPx /  myVideoGpuShow.getWidth() * result[2]) ;
                int height = (int)((float)videoHeightPx /  myVideoGpuShow.getHeight() * result[3] );
                //width , height , x , y
                String filterDes = String.format(Locale.CHINESE , "crop=%d:%d:%d:%d" , width , height , startX , startY) ;
                startDealVideo(listPath.get(0) ,  FileUtils.APP_CROP + System.currentTimeMillis()+".mp4" , filterDes , new int[]{width , height});
                showLoadPorgressDialog("处理中...");
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

    @Override
    public void nativeNotify(String str) {
        super.nativeNotify(str);
        if (!TextUtils.isEmpty(str) && !FFmpegUtils.isShowToastMsg(str) && str.startsWith("metadata:width=")) {
            String[] strs = str.split(",");
            String strWidth = strs[0].replace("metadata:width=", "");
            String strHeight = strs[1].replace("height=", "");
            try {
                videoWidthPx = Integer.parseInt(strWidth);
                videoHeightPx = Integer.parseInt(strHeight);
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
    }

}
