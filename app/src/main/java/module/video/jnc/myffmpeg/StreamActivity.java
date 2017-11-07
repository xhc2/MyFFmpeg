package module.video.jnc.myffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

public class StreamActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_stream);
        findViewById(R.id.bt_stream).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //开始推流
                FFmpegUtils.stream(" input url " , " output url ");
            }
        });

    }
}
