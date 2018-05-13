package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;

//https://blog.csdn.net/u010302327/article/details/78457091
public class ShowVideoShaderActivity extends Activity {

    private SeekBar seekBar ;
    private Button btPlay;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_video_shader);
        seekBar = (SeekBar)findViewById(R.id.seek_bar);
        btPlay = (Button)findViewById(R.id.bt_play_button);

        btPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.showVideoGpuPlayOrPause();
            }
        });
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                FFmpegUtils.showVideoGpuJustPause();
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.e("xhc" ," onPause ");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.e("xhc" ," ondestroy ");
        FFmpegUtils.showVideoGpuDestroy();
    }
}
