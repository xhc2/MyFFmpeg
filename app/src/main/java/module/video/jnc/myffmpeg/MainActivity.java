package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
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
        textView.setText(FFmpegUtils.stringJni());
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


        /**
         * opengl绘制大致流程
         * 1.创建顶点着色器程序
         * 2.创建片段着色器程序
         * 3.创建新的着色器对象-》将程序上传到此对象中-》编译-》取出状态-》将顶点，片段程序连接进opengl程序中
         * -》新建opengl程序对象-》将两个着色器附着在opengl程序中-》然后链接连续就可以了。link
         *
         */
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

    }
}
