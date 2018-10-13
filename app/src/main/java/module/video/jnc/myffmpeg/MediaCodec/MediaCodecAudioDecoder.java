package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

public class MediaCodecAudioDecoder {

    private MediaCodec mediaCodec;
    private final String MIME_TYPE = "audio/mp4a-latm";
    private final static int TIME_INTERNAL = 5;
    private AACCallBack callBack;
    public void setAACCallBack(AACCallBack callBack){
        this.callBack = callBack;
    }

    public interface AACCallBack{
        void callBack(byte[] pcm);
    }

    public MediaCodecAudioDecoder(int sampleRate , int channelCount){
        try {
            mediaCodec = MediaCodec.createDecoderByType(MIME_TYPE);
            MediaFormat mediaFormat = MediaFormat.createAudioFormat(MIME_TYPE , sampleRate , channelCount);
            //用来标记AAC是否有adts头，1->有
            mediaFormat.setInteger(MediaFormat.KEY_IS_ADTS, 1);
            //ByteBuffer key（暂时不了解该参数的含义，但必须设置）
            byte[] data = new byte[]{(byte) 0x11, (byte) 0x90};
            ByteBuffer csd_0 = ByteBuffer.wrap(data);
            mediaFormat.setByteBuffer("csd-0", csd_0);
            mediaCodec.configure(mediaFormat, null, null, 0);
            mediaCodec.start();
        } catch (IOException e) {
            Log.e("xhc" , " init exception "+e.getMessage());
            e.printStackTrace();
        }
    }

    public MediaCodecAudioDecoder(String mimeType , MediaFormat mf){

        try {
            mediaCodec = MediaCodec.createDecoderByType(mimeType);
            mediaCodec.configure(mf, null, null, 0);
            mediaCodec.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public boolean onFrame(byte[] buf, int offset, int length) {
        // 获取输入buffer index
//        ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
        //-1表示一直等待；0表示不等待；其他大于0的参数表示等待毫秒数
        int inputBufferIndex = mediaCodec.dequeueInputBuffer(100);
        if (inputBufferIndex >= 0) {
            ByteBuffer inputBuffer = null;
            if (Build.VERSION.SDK_INT >= 21) {
                inputBuffer = mediaCodec.getInputBuffer(inputBufferIndex);
            } else {
                inputBuffer = mediaCodec.getInputBuffers()[inputBufferIndex];
            }
            if(inputBuffer == null ){
                return false;
            }
            //清空buffer
            inputBuffer.clear();
            //put需要解码的数据
            inputBuffer.put(buf, offset, length);
            //解码
            mediaCodec.queueInputBuffer(inputBufferIndex, 0, length, 0, 0);

        } else {
            return false;
        }

        // 获取输出buffer index
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 100);
        Log.e("xhc", " output index " + outputBufferIndex);
        //循环解码，直到数据全部解码完成
        while (outputBufferIndex >= 0) {
            ByteBuffer outputBuffer = null;
            if (Build.VERSION.SDK_INT >= 21) {
                outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex);
            } else {
                outputBuffer = mediaCodec.getOutputBuffers()[outputBufferIndex];
            }
            if (Build.VERSION.SDK_INT <= 19) {
                outputBuffer.position(bufferInfo.offset);
                outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
            }
            int size = bufferInfo.size;
            Log.e("xhc" , "out size "+size);
            byte[] outBuffer = new byte[size];
            outputBuffer.get(outBuffer);
            if(callBack != null){
                callBack.callBack(outBuffer);
            }
            mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
            outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
        }

        return true;
    }

    public void stopCodec() {

        try {
            mediaCodec.stop();
            mediaCodec.release();
            mediaCodec = null;

        } catch (Exception e) {
            e.printStackTrace();
            mediaCodec = null;
        }
    }
}
