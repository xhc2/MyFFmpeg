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
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyLoadMoreBaseAdapter;
import module.video.jnc.myffmpeg.widget.MyLoadMoreRecycleView;
import module.video.jnc.myffmpeg.widget.MyVideoView;

public class TestActivity extends AppCompatActivity {

    private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test2);
        final MyVideoView videoView = findViewById(R.id.video_view);
        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                videoView.play("sdcard/FFmpeg/video_src/test.mp4");

//                handler.post(new Runnable() {
//                    @Override
//                    public void run() {
//                        Log.e("xhc" , " click start ");
//                        Toast.makeText(TestActivity.this, "toast", Toast.LENGTH_SHORT).show();
//                        try{
//                            Thread.sleep(1000 * 10);
//                        }catch(Exception e){
//
//                        }
//                        Log.e("xhc" , " click end ");
//
//                    }
//                });


            }
        });

        findViewById(R.id.bt_start2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                videoView.play("sdcard/FFmpeg/video_src/time.mp4");
            }
        });


    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }



}
