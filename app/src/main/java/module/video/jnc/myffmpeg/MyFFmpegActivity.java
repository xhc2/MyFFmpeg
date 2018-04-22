package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import java.io.File;

import module.video.jnc.myffmpeg.EGLCamera.EGLCameraActivity;
import module.video.jnc.myffmpeg.opengl.OpenglActivity;

/**
 * http://ffmpeg.org/doxygen/3.4/index.html
 * ffmpeg展示界面
 * 1.对音视频文件的编解码
 * 2.推流先不搞,等服务器rtmp的服务器先搭个简单的再处理
 * 3.用swscale yuv ， rgb转码然后显示
 * 4.视频录制
 * 5.打水印
 * 6.音频录制
 * 7.相机录制，然后将视频打水印
 * 8.
 */
public class MyFFmpegActivity extends AppCompatActivity {

    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_ffmpeg);

        findViewById(R.id.camera_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                startActivity(new Intent(MyFFmpegActivity.this, MyCameraFilter.class));
            }
        });

        findViewById(R.id.bt_decode_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, DecodeEncodeActivity.class));
            }
        });
    }
}




















