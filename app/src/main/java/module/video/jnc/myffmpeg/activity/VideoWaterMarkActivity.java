package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.Locale;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoWaterMarkActivity extends SingleFilterActivity  implements View.OnTouchListener   {

    private ImageView img;
    private TextView tvPosition;
    private float x;
    private float y;
    private int waterX, waterY;
    private String logoPath;
    private Bitmap bitmap;
    private static final int CHANGE_IMAGE = 3;
    private int videoWidthPx;
    private int videoHeightPx;

    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case CHANGE_IMAGE:
                    int imgWidth = (int) (((float) myVideoGpuShow.getWidth() / videoWidthPx) * bitmap.getWidth());
                    int imgHeight = (int) (((float) myVideoGpuShow.getHeight() / videoHeightPx) * bitmap.getHeight());
                    Log.e("xhc", " width " + imgWidth + " height " + imgHeight);
                    FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) img.getLayoutParams();
                    params.width = imgWidth;
                    params.height = imgHeight;
                    img.setLayoutParams(params);
                    break;
            }
            return false;
        }
    });
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_water_mark);
        findViewById();
        init();
    }
    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        img = findViewById(R.id.img_water_mark);
        tvPosition = findViewById(R.id.tv_position);
    }

    protected void init() {
        super.init();
        waterX = 0;
        waterY = 0;

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {

            @Override
            public void clickRight() {
                //右键点击
                FileUtils.makeWaterDir();
                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
                String filterDes = String.format(Locale.CHINESE , "movie=%s[wm];[in][wm]overlay=%d:%d[out]", logoPath, waterX, waterY);
                startDealVideo(listPath.get(0),
                        FileUtils.APP_WATER_MARK + "water_mark" + System.currentTimeMillis() + ".mp4" , filterDes ,  new int[]{-1, -1});
                showLoadPorgressDialog("处理中...");
            }
        });
        myVideoGpuShow.setOnTouchListener(this);
        Intent intent = getIntent();
        if (intent != null) {
            logoPath = intent.getStringExtra("pic");
            bitmap = BitmapFactory.decodeFile(logoPath);
            img.setImageBitmap(bitmap);
        }
    }


    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                break;
            case MotionEvent.ACTION_MOVE:
                x = event.getX();
                y = event.getY();
                if (x < 0) {
                    x = 0;
                }
                if (y < 0) {
                    y = 0;
                }
                if ((x + img.getWidth()) > myVideoGpuShow.getWidth()) {
                    x = myVideoGpuShow.getWidth() - img.getWidth();
                }
                if ((y + img.getHeight()) > myVideoGpuShow.getHeight()) {
                    y = myVideoGpuShow.getHeight() - img.getHeight();
                }

                img.setX((int) x);
                img.setY((int) y);
                waterX = (int) ((x / myVideoGpuShow.getWidth()) * FFmpegUtils.getVideoWidth());
                waterY = (int) ((y / myVideoGpuShow.getHeight()) * FFmpegUtils.getVideoHeight());
                tvPosition.setText(" x坐标： " + waterX + ", y坐标：" + waterY);
                break;
            case MotionEvent.ACTION_UP:
                break;
        }
        return true;
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        bitmap.recycle();
        handler.removeCallbacksAndMessages(null);
    }


    @Override
    public void nativeNotify(String str) {
        super.nativeNotify(str);
        if (!TextUtils.isEmpty(str) && !FFmpegUtils.isShowToastMsg(str) && str.startsWith("metadata:width=")) {
            Log.e("xhc" , " crop activity "+str);
            String[] strs = str.split(",");
            String strWidth = strs[0].replace("metadata:width=", "");
            String strHeight = strs[1].replace("height=", "");
            try {
                videoWidthPx = Integer.parseInt(strWidth);
                videoHeightPx = Integer.parseInt(strHeight);
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
            handler.sendEmptyMessage(CHANGE_IMAGE);
        }
    }
}
