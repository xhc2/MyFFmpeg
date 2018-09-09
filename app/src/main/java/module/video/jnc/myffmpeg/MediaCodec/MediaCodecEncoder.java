package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.util.Log;

import java.nio.ByteBuffer;

import static android.media.MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar;


public class MediaCodecEncoder {

    private MediaCodec mcE;
    private final String videoEncoderType = "video/avc";
    private final int width = 480;
    private final int height = 272;
    private MediaFormat mMf ;
    private ByteBuffer[] inputBuffers;
    private ByteBuffer[] outputBuffers;
    private MediaCodec.BufferInfo mBI;


    public MediaCodecEncoder(){
        try{
            mcE = MediaCodec.createDecoderByType(videoEncoderType);
            mMf = MediaFormat.createVideoFormat(videoEncoderType, width, height);
            mMf.setInteger(MediaFormat.KEY_BIT_RATE, 1500000);
            mMf.setInteger(MediaFormat.KEY_FRAME_RATE, 25);
            mMf.setInteger(MediaFormat.KEY_COLOR_FORMAT, COLOR_FormatYUV420Planar);
            mcE.configure(mMf,null,null,MediaCodec.CONFIGURE_FLAG_ENCODE);
        }catch (Exception e){
            Log.e("xhc", " exception "+e.getMessage());
            e.printStackTrace();
        }

    }

    public void start(){
        mcE.start();
        inputBuffers = mcE.getInputBuffers();
        outputBuffers = mcE.getOutputBuffers();
    }




    /**
     * 向编码器输入数据，此处要求输入YUV420P的数据
     * @param data YUV数据
     * @param len 数据长度
     * @param timestamp 时间戳
     * @return
     */
    public int input(byte[] data,int len,long timestamp){
        int index = mcE.dequeueInputBuffer(0);
        Log.e("xhc","" + index);
        if(index >= 0){
            ByteBuffer inputBuffer = inputBuffers[index];
            inputBuffer.clear();
            if(inputBuffer.capacity() < len){
                mcE.queueInputBuffer(index, 0, 0, timestamp, 0);
                return -1;
            }
            inputBuffer.put(data,0,len);
            mcE.queueInputBuffer(index,0,len,timestamp,0);
        }else{
            return index;
        }
        return 1;
    }



    public int output(byte[] data,int[] len,long[] ts){
        int i = mcE.dequeueOutputBuffer(mBI, 0);
        if(i >= 0){
            if(mBI.size > data.length) return -1;
            outputBuffers[i].position(mBI.offset);
            outputBuffers[i].limit(mBI.offset + mBI.size);
            outputBuffers[i].get(data, 0, mBI.size);
            len[0] = mBI.size ;
            ts[0] = mBI.presentationTimeUs;
            mcE.releaseOutputBuffer(i, false);
        }else if (i == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
            outputBuffers = mcE.getOutputBuffers();
            return 2;
        } else if (i == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
            mMf = mcE.getOutputFormat();
            return 2;
        } else if (i == MediaCodec.INFO_TRY_AGAIN_LATER) {
            return 3;
        }
        return 1;
    }


    public void release(){
        mcE.stop();
        mcE.release();
        mcE = null;
        outputBuffers = null;
        inputBuffers = null;
    }

    public void flush() {
        mcE.flush();
    }

}
