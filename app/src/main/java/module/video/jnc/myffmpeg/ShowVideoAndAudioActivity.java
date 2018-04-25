package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

/**
 * 播放视频，音频的文件
 * 1.先播放两者
 * 2.控制视频的速率
 * 3.控制同步。
 */
public class ShowVideoAndAudioActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_video_and_audio);
        findViewById(R.id.test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.testMyShow();
            }
        });
    }
}
