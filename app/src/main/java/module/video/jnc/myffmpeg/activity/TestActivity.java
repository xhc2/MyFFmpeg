package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.R;

public class TestActivity extends AppCompatActivity {

    String[] paths = {"sdcard/FFmpeg/video/test.mp4", "sdcard/FFmpeg/video/v1080.mp4"/* , "sdcard/FFmpeg/video/flv.flv"*/};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test2);

        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread() {
                    @Override
                    public void run() {
                        super.run();
                        FFmpegUtils.test();

                    }
                }.start();

            }
        });

        findViewById(R.id.bt_end).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.test2();
            }
        });
    }
}
