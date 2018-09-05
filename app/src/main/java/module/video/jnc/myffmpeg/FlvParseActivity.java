package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class FlvParseActivity extends Activity {

    private TextView tvData;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_flv_parse);
        tvData = findViewById(R.id.tv_all_data);
        findViewById(R.id.bt_parse).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String result = FFmpegUtils.flvParse("sdcard/FFmpeg/flv.flv");

                tvData.setText(result);
            }
        });


    }
}
