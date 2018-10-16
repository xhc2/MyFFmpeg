package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.widget.TitleBar;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class VideoClipActivity extends BaseActivity implements GLSurfaceView.Renderer, FFmpegUtils.Lis {

    private TitleBar titleBar;
    private List<String> listPath = new ArrayList<>();
    private MyVideoGpuShow myVideoGpuShow;
    private final int NATIVE_MSG = 1;
    private final int START_VIDEO = 2;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case NATIVE_MSG:
                    String str = (String) msg.obj;
                    showToast(str);
                    break;
                case START_VIDEO:
                    String path = (String) msg.obj;
                    myVideoGpuShow.setPlayPath(path);
                    break;

            }
            return false;
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_deal_video);
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = findViewById(R.id.play_gl_surfaceview);
        initGlSurfaceView();
        Intent intent = getIntent();
        ArrayList<String> tempVideos = intent.getStringArrayListExtra("videos");
        if (tempVideos != null) {
            listPath.addAll(tempVideos);
        }
        if (listPath.size() > 0) {
            startVideo(listPath.get(0));
        }
        FFmpegUtils.addNativeNotify(this);
    }


    private void startVideo(final String path) {
        new Thread() {
            @Override
            public void run() {
                super.run();
                FFmpegUtils.destroyMp4Play();
                Message msg = new Message();
                msg.what = START_VIDEO;
                msg.obj = path;
                handler.sendMessage(msg);
            }
        }.start();

    }

    private void initGlSurfaceView() {
        myVideoGpuShow.setEGLContextClientVersion(2);
        myVideoGpuShow.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        myVideoGpuShow.setRenderer(this);//android 8.0需要设置
        myVideoGpuShow.setRenderMode(RENDERMODE_WHEN_DIRTY);
    }


    @Override
    protected void onResume() {
        super.onResume();
        myVideoGpuShow.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        myVideoGpuShow.onPause();
        if (FFmpegUtils.mp4Pause() == 1) {
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.removeNotify(this);
        FFmpegUtils.destroyMp4Play();
    }

    @Override
    public void nativeNotify(String str) {
        Message msg = new Message();
        msg.what = NATIVE_MSG;
        msg.obj = str;
        handler.sendMessage(msg);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

    }

    @Override
    public void onDrawFrame(GL10 gl) {

    }
}
