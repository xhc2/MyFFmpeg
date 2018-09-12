package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import module.video.jnc.myffmpeg.MediaCodec.MediaCodecAudio;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecVideo;



public class HardCodeActivity extends Activity {

    private   MediaCodecVideo mcv;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hard_code);

        findViewById(R.id.bt_h264_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将yuv编码成h264
                try {
                    encodeYuv();
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });

        findViewById(R.id.bt_aac_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将pcm编码成aac

                try {
                    encodePcm();
                } catch (Exception e) {
                    e.printStackTrace();
                }
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

    private void encodePcm() throws  Exception {
        int channelCount = 2;
        int sampleRate = 44100;
        int pcmSize = 4096;
        final FileOutputStream fos = new FileOutputStream("sdcard/FFmpeg/aac.aac");
        MediaCodecAudio mca = new MediaCodecAudio(sampleRate, channelCount,pcmSize, new MediaCodecAudio.AACCallBack() {
            @Override
            public void aacCallBack(byte[] buffer) {
                try{
                    fos.write(buffer);
                }catch (Exception e){
                    Log.e("xhc" ," exception "+e.getMessage());
                }

            }
        });
        mca.startEncode();

        FileInputStream fis = new FileInputStream(new File("sdcard/FFmpeg/test_2c_441_16.pcm"));
        while(true){
            byte[] buffer = new byte[pcmSize];
            if(fis.read(buffer) == -1){
                break;
            }
            mca.addData(buffer);
        }
    }


    private void encodeYuv() throws Exception{
        int width = 640;
        int height = 360;
        final FileOutputStream fos  = new FileOutputStream("sdcard/FFmpeg/output.264");
        mcv = new MediaCodecVideo(width, height, new MediaCodecVideo.H264CallBack() {
            @Override
            public void H264CallBack(byte[] data) {
                try{
                    fos.write(data);
                }
                catch (Exception e){
                    Log.e("xhc" , " exception "+e.getMessage());
                }

            }
        });
        mcv.startEncode();
        try {
            FileInputStream fis = new FileInputStream(new File("sdcard/FFmpeg/yuv_640_360.yuv"));
            while(true){
                byte[] buffer = new byte[height * width * 3 / 2];
                if(fis.read(buffer) == -1)
                {
                    break;
                }
                mcv.addByte(buffer);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }


    }






}
