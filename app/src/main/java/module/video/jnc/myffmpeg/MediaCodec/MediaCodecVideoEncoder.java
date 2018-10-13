package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;

import java.nio.ByteBuffer;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;


/**
 * 用来编码yuv -> h264
 */
public class MediaCodecVideoEncoder extends Thread {

    private String MIME_TYPE = "video/avc";
    private int width;
    private int height;
    private MediaCodec.BufferInfo info;
    private MediaFormat mf;
    private MediaCodec mediaCodec;
    private H264CallBack callBack;
    private Queue<byte[]> queue = new ConcurrentLinkedQueue<>();
    private boolean runFlag = false;
    private int yuvSize;
    private int frameRate = 24;
    private int bitrate ;
    private AddTrackInter addTrackInter;

    public void addTrack(AddTrackInter addTrackInter){
        this.addTrackInter = addTrackInter;
    }

    public MediaCodecVideoEncoder(int width, int height, H264CallBack callBack) {
        this.width = width;
        this.height = height;
        this.callBack = callBack;
        yuvSize = width * height * 3 / 2;
        init();
    }


    private void init() {
        try {
            bitrate = width * height * 3 * 8 * frameRate / 256;
            info = new MediaCodec.BufferInfo();
            mediaCodec = MediaCodec.createEncoderByType(MIME_TYPE);
            mf = MediaFormat.createVideoFormat(MIME_TYPE, width, height);
            mf.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
            mf.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
            mf.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
            //仅仅uv对应错误解码应该只是花屏而已。
//            COLOR_FormatYUV420SemiPlanar yyyyyyyyvuvu COLOR_FormatYUV420Planar
            mf.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar);

            mediaCodec.configure(mf, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mediaCodec.start();

        } catch (Exception e) {
            Log.e("xhc", " exception " + e.getMessage());
        }
    }


    public void addByte(byte[] data) {
        queue.add(data);
    }

    private void encode(byte[] data) {
        int inputBufferId = mediaCodec.dequeueInputBuffer(1000);
        if (inputBufferId >= 0) {
            ByteBuffer inputBuffer = null;
            if (Build.VERSION.SDK_INT >= 21) {
                inputBuffer = mediaCodec.getInputBuffer(inputBufferId);
            } else {
                inputBuffer = mediaCodec.getInputBuffers()[inputBufferId];
            }
            inputBuffer.clear();
            inputBuffer.put(data);
            //这里放入时间戳的方式都是这种getPTSUs()有些奇怪。不过管用
            mediaCodec.queueInputBuffer(inputBufferId, 0, yuvSize, getPTSUs(), 0);
        }

        int outputBufferId = mediaCodec.dequeueOutputBuffer(info, 1000);
        ByteBuffer outputBuffer = null;
        if (outputBufferId == MediaCodec.INFO_TRY_AGAIN_LATER) {
            // wait 5 counts(=TIMEOUT_USEC x 5 = 50msec) until data/EOS come
        }
        else if (outputBufferId == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
        }
        else if(outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED){
            //混合器需要在这个时机addtrack
            Log.e("xhc" , "video INFO_OUTPUT_FORMAT_CHANGED ");
            if(addTrackInter != null){
                addTrackInter.addTrack(mediaCodec.getOutputFormat());
            }
        }
        else if (outputBufferId < 0) {
            // unexpected status
        }
        else{
            if (Build.VERSION.SDK_INT >= 21) {
                outputBuffer = mediaCodec.getOutputBuffer(outputBufferId);
            } else {
                outputBuffer = mediaCodec.getOutputBuffers()[outputBufferId];
            }
            if (Build.VERSION.SDK_INT <= 19) {
                outputBuffer.position(info.offset);
                outputBuffer.limit(info.offset + info.size);
            }
            prevPresentationTimes = info.presentationTimeUs;
            int size = info.size;
            byte[] outBuffer = new byte[size];
            outputBuffer.get(outBuffer);
            if (callBack != null) {
                callBack.H264CallBack(outBuffer, info, outputBuffer);
            }
            mediaCodec.releaseOutputBuffer(outputBufferId, false);
        }
    }

    public interface H264CallBack {
        void H264CallBack(byte[] data, MediaCodec.BufferInfo info, ByteBuffer buffer);
    }

    public interface AddTrackInter{
        void addTrack(MediaFormat format);
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
            Log.e("xhc", "stop exception " + e.getMessage());
            e.printStackTrace();
        }
    }

    private long prevPresentationTimes;
    private long getPTSUs(){
        long result = System.nanoTime() / 1000;
        if(result < prevPresentationTimes){
            result = (prevPresentationTimes  - result ) + result;
        }
        return result;
    }

    @Override
    public void run() {
        super.run();
        while (runFlag) {
            if (!queue.isEmpty()) {
                byte[] buffer = queue.poll();
                try {
                    Thread.sleep(40);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                encode(buffer);
            }
        }
    }
}
