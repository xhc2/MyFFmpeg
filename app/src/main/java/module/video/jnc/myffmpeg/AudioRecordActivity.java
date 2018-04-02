package module.video.jnc.myffmpeg;

import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;

import java.util.LinkedList;
import java.util.List;

import module.video.jnc.myffmpeg.EGLCamera.CameraManeger;

/**
 * 录取声音，通过ffmpeg转格式，然后保存在本地
 */
public class AudioRecordActivity extends AppCompatActivity {

    private FrameLayout fl;
    private AudioRecord ar;
    private int size = 2048;
    private boolean recordFlag = false;
    private static int frequency = 44100;
    private static int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;//单声道
    private static int EncodingBitRate = AudioFormat.ENCODING_PCM_16BIT;    //音频数据格式：脉冲编码调制（PCM）每个样品16位
    private static List<byte[]> listAudio = new LinkedList<>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_record);

        size = AudioRecord.getMinBufferSize(frequency, channelConfiguration, EncodingBitRate);
        ar = new AudioRecord(MediaRecorder.AudioSource.MIC, frequency, channelConfiguration, EncodingBitRate, size);
        FFmpegUtils.initAudioRecord(Constant.rootFile.getAbsolutePath() + "/my_record_audio_.aac" , size);

        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startReocrdAudio();
                startDealAudio();
            }
        });
    }







    private RecordThread recordThread ;

    private void startReocrdAudio() {
        if (ar.getState() == AudioRecord.STATE_INITIALIZED) {
            Log.e("xhc", "AudioRecord.STATE_INITIALIZED ");
        } else {
            Log.e("xhc", "AudioRecord.STATE_unINITIALIZED ");
        }
        ar.startRecording();
        recordFlag = true;
        recordThread = new RecordThread();
        recordThread.start();

    }

    private void releaseAudioRecord() {
        if (ar != null) {
            recordFlag = false;
            ar.stop();
            ar.release();
            ar = null;
        }
    }

    class RecordThread extends Thread {

        @Override
        public void run() {
            super.run();
            byte[] buffer = new byte[size];
            int read = 0;
            while (recordFlag) {
                read = ar.read(buffer, 0, buffer.length);
                Log.e("xhc", "read " + read);
                if (AudioRecord.ERROR_INVALID_OPERATION != read) {
                    listAudio.add(buffer);
                }
            }
        }
    }

    class DealAudioThread extends Thread {
        boolean flag = false;

        @Override
        public void run() {
            super.run();
            while (flag) {
                if (!listAudio.isEmpty()) {
                    if (FFmpegUtils.encodeAudioRecord(listAudio.get(0)) > 0) {
                        listAudio.remove(0);
                    }
                }
            }
        }
    }


     DealAudioThread daThread;

    private void startDealAudio() {
        daThread = new  DealAudioThread();
        daThread.flag = true;
        daThread.start();
    }

    private void stopDealAudio() {
        if (daThread != null) {
            daThread.flag = false;
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        recordFlag = false;
        stopDealAudio();
        try{
            recordThread.join();
            daThread.join();
        }catch (Exception e){

        }finally {
            FFmpegUtils.closeAudioRecord();
            releaseAudioRecord();
        }


    }
}
