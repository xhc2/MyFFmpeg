package module.video.jnc.myffmpeg.activity;

import android.content.Context;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import junit.framework.Test;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyLoadMoreBaseAdapter;
import module.video.jnc.myffmpeg.widget.MyLoadMoreRecycleView;
import module.video.jnc.myffmpeg.widget.MyVideoView;

public class TestActivity extends AppCompatActivity {

    private FrameLayout contain;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test2);
        contain = findViewById(R.id.fl_contain);
//        final MyVideoView videoView = findViewById(R.id.video_view);
        final MyVideoView mv = new MyVideoView(TestActivity.this);
        FrameLayout.LayoutParams parmas = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT ,
                FrameLayout.LayoutParams.MATCH_PARENT);
        contain.addView(mv , parmas);
        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                mv.play("rtmp://58.200.131.2:1935/livetv/hunantv");
            }
        });

        findViewById(R.id.bt_start2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mv.play("sdcard/FFmpeg/video_src/input.mp4");
            }
        });
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }



}
