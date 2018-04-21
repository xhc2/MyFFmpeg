package module.video.jnc.myffmpeg;

import android.app.Dialog;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import module.video.jnc.myffmpeg.opengl.OpenglActivity;

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

        Log.e("xhc" , FFmpegUtils.stringNative());

        findViewById(R.id.bt_opengl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                startActivity(new Intent(NewMainActivity.this, OpenGlActivity.class));
            }
        });

        findViewById(R.id.bt_ffmpeg).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this, MyFFmpegActivity.class));
            }
        });

        findViewById(R.id.bt_all).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                startActivity(new Intent(NewMainActivity.this, MyAllActivity.class));
            }
        });

        findViewById(R.id.play_activity).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showDialog();
            }

        });

    }


    private Dialog dialog;

    private void createMyDialog() {
        dialog = new Dialog(this);
        dialog.setContentView(R.layout.choise_play_dialog_layout);
        dialog.findViewById(R.id.play_audio).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this, PlayAudioActivity.class));
            }
        });

        dialog.findViewById(R.id.play_mp4).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this, PlayVideoActivity.class));
            }
        });


        dialog.findViewById(R.id.play_MP4_open_gl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this, OpenglActivity.class));
            }
        });
        dialog.findViewById(R.id.play_gl_surfaceview).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(NewMainActivity.this, OpenglActivity.class));
            }
        });

    }

    private void showDialog() {
        if (dialog == null) {
            createMyDialog();
        }
        dialog.show();
    }


}






















