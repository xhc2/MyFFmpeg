package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import java.io.File;

/**
 * 视频编码界面
 */
public class EncodeActivity extends AppCompatActivity {

    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_encode);
        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //开始编码 , mp4 转flv
                FFmpegUtils.encode(rootFile+"/test.mp4" , rootFile.getAbsolutePath());
            }
        });
    }
}
