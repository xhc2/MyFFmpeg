package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;

import module.video.jnc.myffmpeg.MediaCodec.MediaCodecAudio;
import module.video.jnc.myffmpeg.MediaCodec.MediaCodecVideo;


/**
 * https://bigflake.com/mediacodec/
 */
public class HardCodeActivity extends Activity {

    private MediaCodecVideo mcv;
    private TextView tvView;
//    private String h264Path = "sdcard/FFmpeg/test.h264";
//    private String aacPath = "sdcard/FFmpeg/test.aac";
    private   MyMediaMuxer mmm;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hard_code);
        tvView = (TextView) findViewById(R.id.tv_view);
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

            }
        });

        findViewById(R.id.bt_h264_d).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        findViewById(R.id.bt_muxer).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //封装音视频
                muxerVideoAudio();
//                try {
//                    testMuxer();
//                } catch (Exception e) {
//                    e.printStackTrace();
//                }
            }
        });

    }

    //从文件中读取nalu，然后时间戳自己控制，好像时间戳是有问题的
    private void testMuxer() throws Exception{
        MediaMuxer mm = new MediaMuxer("sdcard/FFmpeg/hard_muxer.mp4" ,  MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        MediaFormat mfVideo = MediaFormat.createVideoFormat("video/avc",640 , 360);
        int videoTrack = mm.addTrack(mfVideo);
        mm.start();
        int count = 0;
        while(true){
            byte[] buffer = FFmpegUtils.getNextNalu("sdcard/FFmpeg/test.h264");
            if(buffer == null){
                break;
            }

            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            info.offset = 0;
            info.size = buffer.length;
            info.presentationTimeUs = count * 40;
            count ++;
            byte head = buffer[0];
            int type = head & 0x1f;
            info.flags = 0;
            if (type == 5) {
                info.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
            }
            ByteBuffer byteBuffer = ByteBuffer.allocate(buffer.length);
            byteBuffer.put(buffer);
            mm.writeSampleData(videoTrack , byteBuffer , info);
            Log.e("xhc" , " time "+info.presentationTimeUs );
            Thread.sleep(40);
        }
        Log.e("xhc" , " ending  ");
        mm.stop();
        mm.release();

    }


    private String encodePcm(final boolean writeFlag) throws  Exception {
        String fileName = "aac_hard_encoder.aac";
        int channelCount = 2;
        int sampleRate = 44100;
        int pcmSize = 4096;

        final FileOutputStream fos = new FileOutputStream("sdcard/FFmpeg/"+fileName);

        MediaCodecAudio mca = new MediaCodecAudio(sampleRate , channelCount , pcmSize , new MediaCodecAudio.AACCallBack() {
            @Override
            public void aacCallBack(byte[] buffer , MediaCodec.BufferInfo info) {

                try{
                    if(writeFlag){
                        fos.write(buffer);
                    }
                    if(mmm != null){
                        mmm.writeAudioData(buffer , info);
                    }
                }catch (Exception e){
                    Log.e("xhc" ," exception "+e.getMessage());
                }
            }
        });
        mca.addHeadFlag(writeFlag);
        mca.startEncode();

        FileInputStream fis = new FileInputStream(new File("sdcard/FFmpeg/test_2c_441_16.pcm"));
        while(true){
            byte[] buffer = new byte[pcmSize];
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

        mcv = new MediaCodecVideo(width, height, new MediaCodecVideo.H264CallBack() {
            @Override
            public void H264CallBack(byte[] data , MediaCodec.BufferInfo info , ByteBuffer buffer) {
                try{
                    if(writeFlag){
                        fos.write(data);
                    }

                    if(mmm != null){
                        mmm.writeVideoData(data , info);
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

        } catch (Exception e) {
            Log.e("xhc" , "video encode exception "+e.getMessage());
            e.printStackTrace();
        }
        return fileName;
    }



    private void getEncodeH264(final MyMediaMuxer mmm ){
        int width = 640;
        int height = 360;
        boolean readEndFlag = false;
        MediaCodecVideo mcv = new MediaCodecVideo(width, height, new MediaCodecVideo.H264CallBack() {
            @Override
            public void H264CallBack(byte[] data , MediaCodec.BufferInfo info , ByteBuffer buffer) {
                try{
                    if(mmm != null){
                        mmm.writeVideoData(buffer , info);
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
            Thread.sleep(2000);
            mcv.stopEncode();
            mmm.stopMuxer();
        } catch (Exception e) {
            Log.e("xhc" , "video encode exception "+e.getMessage());
            e.printStackTrace();
        }

    }


    /**
     * 混合音视频文件
     */
    private String muxerVideoAudio(){
        String fileName = "hard_muxer.mp4";
        mmm  = new MyMediaMuxer("sdcard/FFmpeg/"+fileName );
//        mmm.initAudio(44100 , 2);
        mmm.initVideo(640 , 360);
        mmm.startMuxer();
        getEncodeH264(mmm);




        return fileName;
    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
//        if(mcv != null){
//            mcv.stopEncode();
//        }
//        if(mmm != null){
//            mmm.stopMuxer();
//        }

    }
}
