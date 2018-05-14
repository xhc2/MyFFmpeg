package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

//https://blog.csdn.net/u010302327/article/details/78457091
public class ShowVideoShaderActivity extends Activity {

    private SeekBar seekBar ;
    private TextView btPlay;
    private boolean runFlag = false;
    private boolean pauseFlag = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_show_video_shader);
        seekBar = (SeekBar)findViewById(R.id.seek_bar);
        btPlay = (TextView)findViewById(R.id.bt_play_button);
        startThread();

        btPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int flag = FFmpegUtils.showVideoGpuPlayOrPause();
                if(flag == 1){
                    btPlay.setText("播放");
                }
                else{
                    btPlay.setText("暂停");
                }
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                Log.e("xhc" , "  onProgressChanged "+seekBar.getProgress());
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                Log.e("xhc" , "  onStartTrackingTouch "+seekBar.getProgress());
                FFmpegUtils.showVideoGpuJustPause();
                pauseFlag = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.e("xhc" , "  onStopTrackingTouch "+seekBar.getProgress());
                FFmpegUtils.showVideoGpuSeek((double)seekBar.getProgress() / (double)seekBar.getMax());
                pauseFlag = false;
            }
        });
    }
    PositionThread thread;

    private void startThread(){
        stopThread();
        runFlag = true;
        thread = new PositionThread();
        thread.start();
    }

    private void stopThread(){
        runFlag = false;
        thread = null;
    }

    class PositionThread extends Thread{

        @Override
        public void run() {
            super.run();

            while(runFlag){
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if(pauseFlag){
                    continue;
                }
                int position = FFmpegUtils.getPlayPosition();
                Log.e("xhc" ," position %d "+position);
                seekBar.setProgress(position);
            }
        }
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
