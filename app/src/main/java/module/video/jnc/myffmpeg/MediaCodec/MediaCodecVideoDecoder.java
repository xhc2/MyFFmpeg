package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

public class MediaCodecVideoDecoder {

    private int width;
    private int height;
    private String outPath;
    private MediaCodec mediaCodec;
    private Surface surface;
    private final static String MIME_TYPE = "video/avc";
    private int count = 0;
    private final static int TIME_INTERNAL = 5;

    public MediaCodecVideoDecoder(Surface surface, String path, int width, int height) {
        this.height = height;
        this.width = width;
        this.outPath = path;
        this.surface = surface;

        try {
            mediaCodec = MediaCodec.createDecoderByType(MIME_TYPE);
            MediaFormat mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE,
                    width, height);
            //配置MediaFormat以及需要显示的surface
            mediaCodec.configure(mediaFormat, surface, null, 0);
            mediaCodec.start();
        } catch (IOException e) {
            Log.e("xhc", " exception " + e.getMessage());
            e.printStackTrace();
        }

    }


    public boolean onFrame(byte[] buf, int offset, int length) {
        // 获取输入buffer index
//        ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
        //-1表示一直等待；0表示不等待；其他大于0的参数表示等待毫秒数
        int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
        if (inputBufferIndex >= 0) {
            ByteBuffer inputBuffer = null;
            if (Build.VERSION.SDK_INT >= 21) {
                inputBuffer = mediaCodec.getInputBuffer(inputBufferIndex);
            } else {
                inputBuffer = mediaCodec.getInputBuffers()[inputBufferIndex];
            }
            //清空buffer
            inputBuffer.clear();
            //put需要解码的数据
            inputBuffer.put(buf, offset, length);
            //解码
            mediaCodec.queueInputBuffer(inputBufferIndex, 0, length, count * TIME_INTERNAL, 0);
            count++;
        } else {
            return false;
        }

        // 获取输出buffer index
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 100);
        Log.e("xhc", " output index " + outputBufferIndex);
        //循环解码，直到数据全部解码完成
        while (outputBufferIndex >= 0) {
            //logger.d("outputBufferIndex = " + outputBufferIndex);
            //true : 将解码的数据显示到surface上
            mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
            outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
        }
        if (outputBufferIndex < 0) {
            //logger.e("outputBufferIndex = " + outputBufferIndex);
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
