package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import java.io.File;

/**
 * 推流器
 */

public class StreamActivity extends AppCompatActivity {
    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_stream);
        findViewById(R.id.bt_stream).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //开始推流
                FFmpegUtils.stream(rootFile+"/test.flv" , "rtmp://192.168.2.123/live/test");
            }
        });
    }



}
