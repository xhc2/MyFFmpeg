package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import java.nio.ByteBuffer;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class MediaCodecAudio extends Thread {

    private boolean runFlag = false;
    private MediaCodec.BufferInfo info;
    private MediaFormat mf;
    private MediaCodec mediaCodec;
    private int sampleRate ;
    private final String MIME = "audio/mp4a-latm";
    private int bitrate = 200000;
    private Queue<byte[]> queue = new ConcurrentLinkedQueue<>();
    private int pcmSize;
    private int count;
    private long timeInter;
    private long pts;
    private int sampleSize = 16;
    private AACCallBack callBack;
    private  int channelCount;
    private boolean addHead = true;
    public MediaCodecAudio( int sampleRate, int channelCount , int pcmSize,  AACCallBack callBack) {
        try {
            this.pcmSize = pcmSize;
            this.sampleRate = sampleRate;
            this.channelCount = channelCount;
            mediaCodec = MediaCodec.createEncoderByType(MIME);
            info = new MediaCodec.BufferInfo();

            mf = MediaFormat.createAudioFormat(MIME,this.sampleRate, this.channelCount);
            mf.setInteger(MediaFormat.KEY_BIT_RATE, this.bitrate);
            mf.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);

            mediaCodec.configure(mf, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mediaCodec.start();
            timeInter = 1000 * 1000 / sampleRate;
            this.callBack = callBack;
        } catch (Exception e) {
            Log.e("xhc" , " init "+e.getMessage());
        }
    }

    public void addHeadFlag(boolean flag){
        this.addHead = flag;
    }

    public void addData(byte[] buffer) {
        queue.add(buffer);
    }

    private void encode(byte[] data) {
        int inputBufferId = mediaCodec.dequeueInputBuffer(1000);
        if (inputBufferId > 0) {
            count++;
            ByteBuffer inputBuffer = null;
            if (Build.VERSION.SDK_INT >= 21) {
                inputBuffer = mediaCodec.getInputBuffer(inputBufferId);
            } else {
                inputBuffer = mediaCodec.getInputBuffers()[inputBufferId];
            }
            inputBuffer.clear();
            inputBuffer.put(data);
            pts += data.length / 16 * timeInter;
            mediaCodec.queueInputBuffer(inputBufferId, 0, data.length, pts, 0);
        }

        int outputBufferId = mediaCodec.dequeueOutputBuffer(info, 1000);
        if (outputBufferId < 0) {
            return;
        }
        ByteBuffer outputBuffer = null;
        if (Build.VERSION.SDK_INT >= 21) {
            outputBuffer = mediaCodec.getOutputBuffer(outputBufferId);
        } else {
            outputBuffer = mediaCodec.getOutputBuffers()[outputBufferId];
        }
        if (Build.VERSION.SDK_INT <= 19) {
            outputBuffer.position(info.offset);
            outputBuffer.limit(info.offset + info.size);
        }
        int size = info.size;
        byte[] outBuffer = null;
        if(addHead){
            outBuffer = new byte[size + 7];
            outputBuffer.get(outBuffer , 7 , size);
            addADTSToBuffer(outBuffer , size + 7);
        }
        else{
            outBuffer = new byte[size ];
            outputBuffer.get(outBuffer , 0 , size);
        }



        if (callBack != null) {
            count ++ ;
            callBack.aacCallBack(outBuffer ,info );
        }
        mediaCodec.releaseOutputBuffer(outputBufferId, false);
    }

    private byte[] addADTSToBuffer(byte[] aac , int bufferLen){
        int profile = 2;  //AAC LC
        int freqIdx = 4;  //44.1KHz
        int chanCfg = 2;  //CPE
        aac[0] = (byte) 0xFF;
        aac[1] = (byte) 0xF9;
//        profile的值等于 Audio Object Type的值减1.
        aac[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        aac[3] = (byte) (((chanCfg & 3) << 6) + (bufferLen >> 11));
        aac[4] = (byte) ((bufferLen & 0x7FF) >> 3);
        aac[5] = (byte) (((bufferLen & 7) << 5) + 0x1F);
        aac[6] = (byte) 0xFC;
        return aac;
    }

    public void startEncode() {
        runFlag = true;
        this.start();
    }

    public void stopEncode() {
        runFlag = false;
        try {
            join();
            mediaCodec.stop();
            mediaCodec.release();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    public interface AACCallBack {
        void aacCallBack(byte[] buffer , MediaCodec.BufferInfo info);
    }

    @Override
    public void run() {
        super.run();
        while (runFlag) {
            if(!queue.isEmpty()){
                byte[] buffer = queue.poll();
                encode(buffer);
            }
        }
    }
}
