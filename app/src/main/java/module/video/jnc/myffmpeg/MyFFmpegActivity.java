package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;



import java.io.File;


/**
 * http://ffmpeg.org/doxygen/3.4/index.html
 * https://blog.csdn.net/wkw1125/article/details/63807128
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
    boolean openSLFlag = false;
    boolean openSLCreateFlag = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_ffmpeg);
//        FFmpegUtils.stringNative();
        Log.e("xhc" , FFmpegUtils.getVersionString()+" TEST ");
        findViewById(R.id.camera_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                startActivity(new Intent(MyFFmpegActivity.this, MyCameraFilter.class));
            }
        });

        findViewById(R.id.bt_opensl_test).setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                if(!openSLCreateFlag){
                    openSLFlag = true;
                    openSLCreateFlag = true;
                    FFmpegUtils.openSLTest(Constant.rootFile.getAbsolutePath()+"/test_sl.pcm");
                }
                else{
                    openSLFlag = !openSLFlag ;
                    FFmpegUtils.openSLPauseOrPlay(openSLFlag);
                }

            }
        });

        findViewById(R.id.bt_video_audio_play_open_gl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //gpu显示视频
                startActivity(new Intent(MyFFmpegActivity.this, ShowVideoShaderActivity.class));
            }
        });

        findViewById(R.id.bt_decode_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //编解码
                startActivity(new Intent(MyFFmpegActivity.this, DecodeEncodeActivity.class));
            }
        });

        findViewById(R.id.bt_video_audio_play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //cpu显示的视频
                startActivity(new Intent(MyFFmpegActivity.this, ShowVideoAndAudioActivity.class));
            }
        });

        findViewById(R.id.bt_pthred_test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //原生线程测试。
                FFmpegUtils.testNativeThread();
            }
        });

        findViewById(R.id.bt_cplusplus_thread_test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //c++原生线程测试
                FFmpegUtils.testCPlusPlusThread();
            }
        });

        findViewById(R.id.bt_sound_touch).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegUtils.openSLDestroy();
    }
}




















