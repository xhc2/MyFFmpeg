package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.media.MediaCodec;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;

import module.video.jnc.myffmpeg.MediaCodec.MediaCodecAudioDecoder;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecAudioEncoder;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecVideoDecoder;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecVideoEncoder;


/**
 * https://bigflake.com/mediacodec/
 */
public class HardCodeActivity extends Activity implements SurfaceHolder.Callback {

    private TextView tvView;
    private SurfaceView surfaceView;
    private SurfaceHolder holder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hard_code);
        tvView = (TextView) findViewById(R.id.tv_view);
        surfaceView = (SurfaceView)findViewById(R.id.surface_view);
        holder = surfaceView.getHolder();
        holder.addCallback(this);
        findViewById(R.id.bt_h264_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将yuv编码成h264
                muxerVideoAudio();
                try {
                    String name = encodeYuv(true);
                    tvView.setText(" 完成 "+name);
                } catch (Exception e) {
                    tvView.setText("异常"+e.getMessage());
                    e.printStackTrace();
                }

            }
        });

        findViewById(R.id.bt_aac_e).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //将pcm编码成aac
                try {
                    String name = encodePcm(true);
                    tvView.setText("完成 "+name);
                } catch (Exception e) {
                    tvView.setText("异常"+e.getMessage());
                    e.printStackTrace();
                }
            }
        });

        findViewById(R.id.bt_h264_d).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int width = 640 ;
                int height = 360;
                String path = "sdcard/FFmpeg/hard_decoder_h264.264";
                MediaCodecVideoDecoder mediaCodecVideoDecoder =
                        new MediaCodecVideoDecoder(holder.getSurface() , path , width , height );

                while(true){
                    byte[] buffer = FFmpegUtils.getNextNalu("sdcard/FFmpeg/test.h264");
                    if(buffer == null){
                        Log.e("xhc" , " read nalu end ");
                        break;
                    }
                    mediaCodecVideoDecoder.onFrame(buffer , 0 , buffer.length);
                }
            }
        });

        findViewById(R.id.bt_aac_d).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MediaCodecAudioDecoder mad = new MediaCodecAudioDecoder(44100 , 2);
                try {
                    byte[] buffer = new byte[200];
                    FileInputStream fis = new FileInputStream("sdcard/FFmpeg/test.aac");
                    int len ;
                    while((len = fis.read(buffer)) != -1){
                        Log.e("xhc" , " aac read size "+len);
                        mad.onFrame(buffer , 0 , len);
                    }

                } catch (Exception e) {
                    Log.e("xhc" , "excepton "+e.getMessage());
                    e.printStackTrace();
                }
            }
        });

        findViewById(R.id.bt_muxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //封装音视频
                muxerVideoAudio();
            }
        });

    }




    private String encodePcm(final boolean writeFlag) throws  Exception {
        String fileName = "aac_hard_encoder.aac";
        int channelCount = 2;
        int sampleRate = 44100;
        int pcmSize = 4096;

        final FileOutputStream fos = new FileOutputStream("sdcard/FFmpeg/"+fileName);

        MediaCodecAudioEncoder mca = new MediaCodecAudioEncoder(sampleRate , channelCount , pcmSize , new MediaCodecAudioEncoder.AACCallBack() {
            @Override
            public void aacCallBack(byte[] buffer , MediaCodec.BufferInfo info , ByteBuffer outputBuffer ) {

                try{
                    if(writeFlag){
                        fos.write(buffer);
                    }

                }catch (Exception e){
                    Log.e("xhc" ," exception "+e.getMessage());
                }
            }
        });
        mca.addHeadFlag(writeFlag);
        mca.startEncode();

        FileInputStream fis = new FileInputStream(new File("sdcard/FFmpeg/test_2c_441_16.pcm"));
        byte[] buffer = new byte[pcmSize];
        while(true){

            if(fis.read(buffer) == -1){
                break;
            }
            mca.addData(buffer);
        }
        return fileName;
    }


    private String encodeYuv(final boolean writeFlag) throws Exception{
        //现在解析出来有花屏的问题。先不管，是编码器和yuv格式没对应上的原因。
        String fileName = "output_hard_encoder.264";
        int width = 640;
        int height = 360;
        final FileOutputStream fos  = new FileOutputStream("sdcard/FFmpeg/"+fileName);

        MediaCodecVideoEncoder mcv = new MediaCodecVideoEncoder(width, height, new MediaCodecVideoEncoder.H264CallBack() {
            @Override
            public void H264CallBack(byte[] data , MediaCodec.BufferInfo info , ByteBuffer buffer) {
                try{
                    if(writeFlag){
                        fos.write(data);
                    }
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
                if( fis.read(buffer) == -1 )
                {
                    break;
                }
                mcv.addByte(buffer);
            }
            fis.close();

        } catch (Exception e) {
            Log.e("xhc" , "video encode exception "+e.getMessage());
            e.printStackTrace();
        }
        return fileName;
    }


    /**
     * 混入就用摄像头的吧
     */
    private String muxerVideoAudio(){


        return "";
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
