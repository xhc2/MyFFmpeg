package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private TextView textView;
    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "Jnc/material/");
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView)findViewById(R.id.tv);
        textView.setText(FFmpegUtils.stringJni());

        findViewById(R.id.start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.decode(rootFile+"/b.mp4" , rootFile+"/decode.yuv");
            }
        });
    }
}
