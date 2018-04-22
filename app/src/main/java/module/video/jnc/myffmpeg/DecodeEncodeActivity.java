package module.video.jnc.myffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class DecodeEncodeActivity extends AppCompatActivity {

    private TextView tv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decode_encode);

        tv = (TextView)findViewById(R.id.tv_warn);

        findViewById(R.id.bt_decode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                findViewById(R.id.bt_decode).setEnabled(false);
                tv.setText("解码开始");
                FFmpegUtils.decodeMp4ToYuvPcm(Constant.rootFile.getAbsolutePath()+"/test.MP4");
                tv.setText("解码结束");
            }
        });
        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                findViewById(R.id.bt_encode).setEnabled(false);

            }
        });


    }
}
