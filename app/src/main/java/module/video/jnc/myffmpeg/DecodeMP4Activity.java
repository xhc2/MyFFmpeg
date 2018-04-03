package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class DecodeMP4Activity extends AppCompatActivity {

    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
    private TextView tv ;
    private boolean flag = false;


    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {

            switch (msg.what){
                case 1:
                    flag = true;
                    tv.setText("解码中...");
                    break;
                case 2:
                    flag = true;
                    tv.setText("编码中...");
                    break;
                case 3:
                    flag = false;
                    tv.setText("完成...");
                    break;
            }

            return false;
        }
    });
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_decode_mp4);
        tv = findViewById(R.id.tv);


        findViewById(R.id.decode_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new DealThread(1).start();
            }
        });

        findViewById(R.id.encode_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new DealThread(2).start();
            }
        });

        findViewById(R.id.play).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(DecodeMP4Activity.this , "播放yuv的功能还在制作中" , Toast.LENGTH_SHORT).show();
            }
        });
    }

    private class DealThread extends Thread{
        int encodeFlag = 0;
        DealThread(int flag){
            this.encodeFlag = flag;
        }

        @Override
        public void run() {
            super.run();
            switch (encodeFlag){
                case 1:
                    //解码

                    handler.sendEmptyMessage(1);
                    FFmpegUtils.decode(rootFile+"/test.mp4" , rootFile+"/decode.yuv");
                    handler.sendEmptyMessage(3);
                    break;
                case 2:
                    //编码
                    handler.sendEmptyMessage(2);
                     FFmpegUtils.encodeYuv(Constant.rootFile.getAbsolutePath()+"/test_480_272.yuv" ,
                            Constant.rootFile.getAbsolutePath()+"/encodeyuv.MP4");
                    handler.sendEmptyMessage(3);
                    break;
            }

        }
    }

    @Override
    public void onBackPressed() {
        if(!flag){
            super.onBackPressed();
        }
        else{
            Toast.makeText(DecodeMP4Activity.this , "还在处理中..." ,Toast.LENGTH_SHORT).show();
        }

    }
}
