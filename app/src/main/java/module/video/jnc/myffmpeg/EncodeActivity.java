package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

/**
 * 视频编码界面
 */
public class EncodeActivity extends AppCompatActivity {

    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "FFmpeg/");
    private TextView tv;
    private boolean flag = false;
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            switch (message.what){
                case 1:
                    flag = true;
                    tv.setText("开始换中...");
                    break;
                case 2:
                    flag = false;
                    tv.setText("完成");
                    break;
            }

            return false;
        }
    });
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_encode);
        tv = (TextView) findViewById(R.id.tv);
        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //开始编码 , mp4 转flv
            new DealThread().start();
            }
        });
    }

    private class DealThread extends Thread{
        @Override
        public void run() {
            super.run();
            handler.sendEmptyMessage(1);
//            FFmpegUtils.encode(rootFile+"/test.mp4" , rootFile.getAbsolutePath());
            handler.sendEmptyMessage(2);
        }
    }

    @Override
    public void onBackPressed() {
        if(!flag){
            super.onBackPressed();
        }
        else{
            Toast.makeText(EncodeActivity.this , "还在处理中" , Toast.LENGTH_SHORT).show();
        }

    }
}
