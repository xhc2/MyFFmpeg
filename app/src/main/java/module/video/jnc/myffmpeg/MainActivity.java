package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

/**
 * 参考：
 * http://ffmpeg.org/doxygen/3.2/index.html
 * http://blog.csdn.net/leixiaohua1020/article/details/25430425
 * http://www.iqiyi.com/paopao/u/2044178079/
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
    }
}
