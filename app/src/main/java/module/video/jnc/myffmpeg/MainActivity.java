package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;

import module.video.jnc.myffmpeg.EGLCamera.EGLCameraActivity;
import module.video.jnc.myffmpeg.opengl.OpenglActivity;

/**
 * 参考：
 * http://ffmpeg.org/doxygen/3.2/index.html
 * http://blog.csdn.net/leixiaohua1020/article/details/25430425
 * http://www.iqiyi.com/paopao/u/2044178079/
 * http://blog.csdn.net/gobitan/article/details/24771311
 * https://www.gitbook.com/book/xdsnet/other-doc-cn-ffmpeg/details 中文文档
 * http://blog.51cto.com/ticktick/category17.html
 *
 * 1.将视频和音频分离，再复用。（比如a视频的画面，b的声音）
 * 2.推流：rtmp
 * 3.水印：时间戳，图片
 * 4.转码mp4转avi等
 * 5.裁剪，yuv转rgb等，改变分辨率
 * 6.通过摄像头录入画面，打水印，或者推流，或者加入其它音频的声音，或者将声音改变，音调变高等。
 * 7.通过sdl播放，opengl（不知道怎么使用）
 */
public class MainActivity extends AppCompatActivity {

    private TextView textView;
    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView)findViewById(R.id.tv);
//        textView.setText(FFmpegUtils.stringJni());
        if(!rootFile.exists()){
            //创建文件夹
            rootFile.mkdirs();
        }
        findViewById(R.id.decode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  DecodeMP4Activity.class));
            }
        });
        findViewById(R.id.stream).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  StreamActivity.class));
            }
        });
        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  EncodeActivity.class));
            }
        });

        findViewById(R.id.encode_yuv).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  EncodeYuvActivity.class));
            }
        });

        findViewById(R.id.bt_sdl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  SDLActivity.class));
            }
        });
        findViewById(R.id.bt_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  AddFilterActivity.class));
            }
        });
        findViewById(R.id.bt_transcoding).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.transcodeing(rootFile+"/test.mp4" , rootFile+"/trancoding.avi");
            }
        });
        findViewById(R.id.bt_swscale).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.swscale(rootFile+"/test.yuv" , rootFile+"/swscale.rgb");
            }
        });
        findViewById(R.id.bt_muxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  MuxerActivity.class));
            }
        });

        findViewById(R.id.bt_opengl_texure).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  OpenGlTureActivity.class));
            }
        });
        findViewById(R.id.bt_opengl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this,  OpenglActivity.class));
            }
        });

        findViewById(R.id.eglcamera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this , EGLCameraActivity.class));
            }
        });
        findViewById(R.id.ffmpeg_camera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this , MyRecordActivity.class));
            }
        });
        findViewById(R.id.test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this , TestActivity.class));
            }
        });
        findViewById(R.id.camera_ffmpeg).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this , MyNewRecordActivity.class));
            }
        });
//        new Thread(new MyRunable()).start();

    }
    byte[] b ={1 , 2 , 3};
    class MyRunable implements Runnable{
        @Override
        public void run() {
            //测试数组的几个参数，在c文件查看吧。
            for(int j = 0 ; j < 1000000000 ; ++ j){
                FFmpegUtils.testArray(b);
                for(int i = 0 ;i < 3 ; ++i){
                    Log.e("xhc_jni" , b[i]+" ? ");
                }
            }

        }
    }

}
