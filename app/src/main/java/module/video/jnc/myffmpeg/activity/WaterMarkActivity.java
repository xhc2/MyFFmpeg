//package module.video.jnc.myffmpeg.activity;
//
//import android.content.Intent;
//import android.graphics.Bitmap;
//import android.graphics.BitmapFactory;
//import android.os.Handler;
//import android.os.Message;
//import android.support.v7.app.AppCompatActivity;
//import android.os.Bundle;
//import android.text.TextUtils;
//import android.util.Log;
//import android.view.MotionEvent;
//import android.view.View;
//import android.widget.FrameLayout;
//import android.widget.ImageView;
//import android.widget.RelativeLayout;
//import android.widget.TextView;
//
//import module.video.jnc.myffmpeg.FFmpegUtils;
//import module.video.jnc.myffmpeg.MyRender;
//import module.video.jnc.myffmpeg.MyVideoGpuShow;
//import module.video.jnc.myffmpeg.R;
//import module.video.jnc.myffmpeg.tool.FileUtils;
//import module.video.jnc.myffmpeg.widget.TitleBar;
//
//import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;
//
///**
// * 水印控件 / myVideoGpuShow = 水印 / 视频宽度。这样水印的位置才能放的准确
// * const char *filter_descr = ;
// */
//public class WaterMarkActivity extends VideoEditParentActivity implements View.OnTouchListener, FFmpegUtils.Lis {
//
//    private ImageView img;
//    private TextView tvPosition;
//    private float x;
//    private float y;
//    private int waterX, waterY;
//    private String logoPath;
//    private Bitmap bitmap;
//    private static final int PROGRESS = 2;
//    private static final int CHANGE_IMAGE = 3;
//    private int videoWidthPx;
//    private int videoHeightPx;
//
//    private Handler handler = new Handler(new Handler.Callback() {
//        @Override
//        public boolean handleMessage(Message msg) {
//            switch (msg.what) {
//                case PROGRESS:
//                    if (progress == 100) {
//                        dismissLoadPorgressDialog();
//                        showToast("已完成");
//                        stopProgressThread();
//                        FFmpegUtils.videoFilterDestroy();
//                        break;
//                    }
//                    setLoadPorgressDialogProgress(progress);
//                    break;
//                case CHANGE_IMAGE:
//
//                    int imgWidth = (int) (((float) myVideoGpuShow.getWidth() / videoWidthPx) * bitmap.getWidth());
//                    int imgHeight = (int) (((float) myVideoGpuShow.getHeight() / videoHeightPx) * bitmap.getHeight());
//                    Log.e("xhc", " width " + imgWidth + " height " + imgHeight);
//                    FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) img.getLayoutParams();
//                    params.width = imgWidth;
//                    params.height = imgHeight;
//                    img.setLayoutParams(params);
//                    break;
//            }
//            return false;
//        }
//    });
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_water_mark);
//        findViewById();
//        init();
//    }
//
//    private void findViewById() {
//        titleBar = findViewById(R.id.title);
//        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
//        img = findViewById(R.id.img_water_mark);
//        tvPosition = findViewById(R.id.tv_position);
//    }
//
//    protected void init() {
//        super.init();
//        waterX = 0;
//        waterY = 0;
//        FFmpegUtils.addNativeNotify(this);
//        titleBar.setRightClickInter(new TitleBar.RightClickInter() {
//            @Override
//            public void clickRight() {
//                //右键点击
//
//                if (dealFlag) {
//                    showToast("正在处理中，请稍等");
//                    showLoadPorgressDialog("处理中...");
//                    return;
//                }
//                startWaterMarkThread();
//                startProgressThread();
//                showLoadPorgressDialog("处理中...");
//            }
//        });
//        myVideoGpuShow.setOnTouchListener(this);
//        Intent intent = getIntent();
//        if (intent != null) {
//            logoPath = intent.getStringExtra("pic");
//            bitmap = BitmapFactory.decodeFile(logoPath);
//            img.setImageBitmap(bitmap);
//        }
//    }
//
//
//    @Override
//    public void onWindowFocusChanged(boolean hasFocus) {
//        super.onWindowFocusChanged(hasFocus);
//
//        if (!activityFoucsFlag & hasFocus && listPath.size() > 0) {
//            activityFoucsFlag = true;
//            startPlayThread(listPath.get(0));
//        }
//    }
//
//    @Override
//    public boolean onTouch(View v, MotionEvent event) {
//        switch (event.getAction()) {
//            case MotionEvent.ACTION_DOWN:
//                break;
//            case MotionEvent.ACTION_MOVE:
//                x = event.getX();
//                y = event.getY();
//                if (x < 0) {
//                    x = 0;
//                }
//                if (y < 0) {
//                    y = 0;
//                }
//                if ((x + img.getWidth()) > myVideoGpuShow.getWidth()) {
//                    x = myVideoGpuShow.getWidth() - img.getWidth();
//                }
//                if ((y + img.getHeight()) > myVideoGpuShow.getHeight()) {
//                    y = myVideoGpuShow.getHeight() - img.getHeight();
//                }
//
//                img.setX((int) x);
//                img.setY((int) y);
//                waterX = (int) ((x / myVideoGpuShow.getWidth()) * FFmpegUtils.getVideoWidth());
//                waterY = (int) ((y / myVideoGpuShow.getHeight()) * FFmpegUtils.getVideoHeight());
//                tvPosition.setText(" x坐标： " + waterX + ", y坐标：" + waterY);
//                break;
//            case MotionEvent.ACTION_UP:
//                break;
//        }
//        return true;
//    }
//
//    //打水印线程相关
//    WaterMarkThread waterMarkThread;
//
//    private void startWaterMarkThread() {
//        stopWaterMarkThread();
//        waterMarkThread = new WaterMarkThread();
//        waterMarkThread.start();
//    }
//
//    private void stopWaterMarkThread() {
//        FFmpegUtils.videoFilterDestroy();
//        if (waterMarkThread != null) {
//            try {
//                waterMarkThread.join();
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//            waterMarkThread = null;
//        }
//    }
//
//    class WaterMarkThread extends Thread {
//        @Override
//        public void run() {
//            super.run();
//            FileUtils.makeWaterDir();
//            dealFlag = true;
//
////            Log.e("xhc", "filter Des: " + filterDes);
////            FFmpegUtils.initVideoFilter(, filterDes, new int[]{-1, -1});
//            FFmpegUtils.videoFilterStart();
//
//            dealFlag = false;
//        }
//    }
//
//    //查看水印进度相关
//    private ProgressThread progressThread;
//
//    private void startProgressThread() {
//        stopProgressThread();
//        progressThread = new ProgressThread();
//        progressThread.progressFlag = true;
//        progressThread.start();
//    }
//
//    private void stopProgressThread() {
//        if (progressThread != null) {
//            progressThread.progressFlag = false;
//            try {
//                progressThread.join();
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//            progressThread = null;
//        }
//    }
//
//    class ProgressThread extends Thread {
//        boolean progressFlag = false;
//
//        @Override
//        public void run() {
//            super.run();
//            while (progressFlag) {
//                progress = FFmpegUtils.getVideoFilterProgress();
//                handler.sendEmptyMessage(PROGRESS);
//                try {
//                    sleep(1000);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
//            }
//        }
//    }
//
//    @Override
//    protected void onDestroy() {
//        super.onDestroy();
//        bitmap.recycle();
//        stopWaterMarkThread();
//        stopProgressThread();
//        FFmpegUtils.removeNotify(this);
//        handler.removeCallbacksAndMessages(null);
//    }
//
//
//    @Override
//    public void nativeNotify(String str) {
//        if (!TextUtils.isEmpty(str) && !FFmpegUtils.isShowToastMsg(str) && str.startsWith("metadata:width=")) {
//            String[] strs = str.split(",");
//            String strWidth = strs[0].replace("metadata:width=", "");
//            String strHeight = strs[1].replace("height=", "");
//            try {
//                videoWidthPx = Integer.parseInt(strWidth);
//                videoHeightPx = Integer.parseInt(strHeight);
//            } catch (NumberFormatException e) {
//                e.printStackTrace();
//            }
//            handler.sendEmptyMessage(CHANGE_IMAGE);
//        }
//    }
//}
