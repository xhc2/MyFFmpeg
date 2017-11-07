package module.video.jnc.myffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import java.io.File;

public class DecodeMP4Activity extends AppCompatActivity {

    private File rootFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + File.separator + "Jnc/material/");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decode_mp4);
        findViewById(R.id.start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.decode(rootFile+"/b.mp4" , rootFile+"/decode.yuv");
            }
        });

    }
}
