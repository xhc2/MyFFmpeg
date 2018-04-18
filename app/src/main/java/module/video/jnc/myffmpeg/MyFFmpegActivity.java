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
                startActivity(new Intent(MyFFmpegActivity.this, MyCameraFilter.class));
            }
        });

        findViewById(R.id.decode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, DecodeMP4Activity.class));
            }
        });
        findViewById(R.id.stream).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, StreamActivity.class));
            }
        });
        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, EncodeActivity.class));
            }
        });

        findViewById(R.id.bt_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, AddFilterActivity.class));
            }
        });
        findViewById(R.id.bt_swscale).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.swscale(rootFile + "/test.yuv", rootFile + "/swscale.rgb");
            }
        });
        findViewById(R.id.bt_muxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, MuxerActivity.class));
            }
        });

        findViewById(R.id.ffmpeg_camera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, MyRecordActivity.class));
            }
        });

        findViewById(R.id.camera_ffmpeg).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, MyNewRecordActivity.class));
            }
        });

        findViewById(R.id.audio_record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, AudioRecordActivity.class));
            }
        });

        findViewById(R.id.test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MyFFmpegActivity.this, TestActivity.class));
            }
        });
    }
}
