package module.video.jnc.myffmpeg;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

/**
 * 新首页展示
 * 分成opengl模块
 * ffmpeg模块
 * 结合模块。一个小应用
 */
public class NewMainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_new_main);

        findViewById(R.id.bt_opengl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this , OpenGlActivity.class));
            }
        });

        findViewById(R.id.bt_ffmpeg).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this , MyFFmpegActivity.class));
            }
        });

        findViewById(R.id.bt_all).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this , MyAllActivity.class));
            }
        });

        findViewById(R.id.play_activity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this,  PlayVideoActivity.class));
            }
        });

    }
}






















