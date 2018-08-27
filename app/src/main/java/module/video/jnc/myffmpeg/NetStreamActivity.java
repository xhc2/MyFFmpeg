package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

/**
 * https://blog.csdn.net/leixiaohua1020/article/details/39803457
 * rtmp 网络流
 */
public class NetStreamActivity extends Activity implements FFmpegUtils.Lis {


    private EditText etUrl;

    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what){
                case 1:
                    Toast.makeText(NetStreamActivity.this, (String)msg.obj , Toast.LENGTH_LONG).show();
                    break;
            }
            return false;
        }
    });

    private String ouputPath = "rtmp://192.168.2.104/live/live";
//    private String ouputPath = "sdcard/FFmpeg/cameraStream.flv";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_net_stream);

        etUrl = (EditText)findViewById(R.id.et_url);
        etUrl.setText(ouputPath);

        FFmpegUtils.addNativeNotify(this);

        findViewById(R.id.bt_camera_stream).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(NetStreamActivity.this , CameraStreamActivity.class);
                intent.putExtra("outputpath" , ouputPath);
                startActivity(intent);
            }
        });

        findViewById(R.id.bt_net_work).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //注意 后面的live 是name
                FFmpegUtils.rtmpInit(ouputPath ,
                        "sdcard/FFmpeg/video/test.flv");
            }
        });

        findViewById(R.id.bt_srs_librtmp).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.srsTest(etUrl.getText().toString());
            }
        });

        findViewById(R.id.bt_test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //测试代码
                FFmpegUtils.test();
            }
        });
    }


    @Override
    public void nativeNotify(String str) {
        Message msg = new Message();
        msg.what = 1;
        msg.obj = str;
        handler.sendMessage(msg);
    }

    @Override
    protected void onPause() {
        super.onPause();
        FFmpegUtils.rtmpClose();
    }
}




