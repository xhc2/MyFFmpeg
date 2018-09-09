package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.media.MediaCodec;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.io.FileInputStream;

import module.video.jnc.myffmpeg.MediaCodec.MediaCodecEncoder;


public class HardCodeActivity extends Activity {

    private static int yuvSize = (int)(480 * 272 * 1.5 );
    private MediaCodecEncoder mce = null;
    private String inputPath = "sdcard/FFmpeg/dc_480_272.yuv";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hard_code);
        findViewById(R.id.bt_h264_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将yuv编码成h264
                mce = new MediaCodecEncoder();
                mce.start();
                EncoderInputThread eit = new EncoderInputThread();
                eit.start();


            }
        });

        findViewById(R.id.bt_aac_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将yuv编码成h264
            }
        });

        findViewById(R.id.bt_h264_d).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        findViewById(R.id.bt_h264_d).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });
    }


    class EncoderInputThread extends Thread{

        private int count = 0 ;
        private int timeInter = 0;
        EncoderInputThread(){
            timeInter = 1000 / 25;
        }

        @Override
        public void run() {
            super.run();
            FileInputStream fis = null;
            byte[] buffer = new byte[yuvSize];
            try{
                fis = new FileInputStream(new File(inputPath));
                int len = -1;
                while((len =  fis.read(buffer)) !=-1){
                    //送入编码
                    int re = mce.input(buffer ,len , count * timeInter);
                    Log.e("xhc" , " input result "+re);
                    count ++;
                }
            }
            catch (Exception e){
                Log.e("xhc" , " msg "+e.getMessage());
            }
        }
    }



}
