package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

/**
 * https://blog.csdn.net/leixiaohua1020/article/details/39803457
 * rtmp 网络流
 */
public class NetStreamActivity extends Activity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_net_stream);


        findViewById(R.id.bt_net_work).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //注意 后面的live 是name
                FFmpegUtils.rtmpInit("rtmp://192.168.2.109/live/live"/*"sdcard/FFmpeg/aaaaa.mp4"*/ ,
                        "sdcard/FFmpeg/video/test.flv");
            }
        });
    }


    @Override
    protected void onPause() {
        super.onPause();
        FFmpegUtils.rtmpClose();
    }
}




