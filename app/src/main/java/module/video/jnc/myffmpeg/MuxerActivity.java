package module.video.jnc.myffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

/**
 * 分离器，复用器
 */
public class MuxerActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_muxer);

        findViewById(R.id.muxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.muxer(Constant.rootFile.getAbsolutePath()+"/sintel.ts" ,Constant.rootFile.getAbsolutePath()+"/sintel_demuxer.h264"
                        , Constant.rootFile.getAbsolutePath()+"/sintel_demuxer.aac"  );
            }
        });
        findViewById(R.id.demuxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.demuxer(Constant.rootFile.getAbsolutePath()+"/sintel.ts" ,Constant.rootFile.getAbsolutePath()+"/sintel_demuxer.h264"
                        , Constant.rootFile.getAbsolutePath()+"/sintel_demuxer.aac"  );
            }
        });
    }
}
