package module.video.jnc.myffmpeg;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.MediaCodec.MuxerData;

import static android.media.MediaCodec.BUFFER_FLAG_SYNC_FRAME;

/**
 * 混合文件
 */
public class MyMediaMuxer extends Thread {

    public String path;
    private boolean flag;
    private MediaMuxer muxer;
    private MediaFormat mfVideo;
    private MediaFormat mfAudio;
    private String MIME_TYPE = "video/avc";
    private final String MIME_AUDIO = "audio/mp4a-latm";
    private int videoTrack;
    private int audioTrack;
    private String h264Path;
    private String aacPath;
    private List<MuxerData> queue = new ArrayList<>();

    public MyMediaMuxer(String outpath) {
        this.path = outpath;
//        this.h264Path = h264Path;
//        this.aacPath = aacPath;
        try {
            muxer = new MediaMuxer(path, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException e) {
            Log.e("xhc", " excepton " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void initAudio(int sampleRate, int channelCount) {
        mfAudio = MediaFormat.createAudioFormat(MIME_AUDIO, sampleRate, channelCount);
        audioTrack = muxer.addTrack(mfAudio);
        Log.e("xhc", " audioTrack " + audioTrack);
    }

    public void initVideo(int width, int height) {
        mfVideo = MediaFormat.createVideoFormat(MIME_TYPE, width, height);
        videoTrack = muxer.addTrack(mfVideo);
        Log.e("xhc", " videoTrack " + videoTrack);
    }


    public void startMuxer() {
        muxer.start();
        flag = true;
        this.start();
    }

    public void stopMuxer() {
        flag = false;
        try {
            this.join();
        } catch (Exception e) {
            e.printStackTrace();
        }
        muxer.stop();
        muxer.release();
        Log.e("xhc", " muxer release ");
    }

    public void writeVideoData(byte[] buffer, MediaCodec.BufferInfo info) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(buffer.length);
        byteBuffer.put(buffer);
        MuxerData md = new MuxerData();
        md.isAudio = false;
        md.byteBuffer = byteBuffer;
        md.info = info;
        byte head = buffer[0];
        int type = head & 0x1f;
        md.info.flags = 0;
        if (type == 5) {
            md.info.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
        }
        queue.add(md);
    }

    public void writeVideoData(ByteBuffer buffer ,  MediaCodec.BufferInfo info){
        MuxerData md = new MuxerData();
        md.isAudio = false;
        md.byteBuffer = buffer;
        md.info = info;
        queue.add(md);
    }

    public void writeAudioData(byte[] buffer, MediaCodec.BufferInfo info) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(buffer.length);
        byteBuffer.put(buffer);
        MuxerData md = new MuxerData();
        md.isAudio = true;
        md.byteBuffer = byteBuffer;
        md.info = info;
        queue.add(md);
    }

    @Override
    public void run() {
        super.run();

        while (flag) {

            if (!queue.isEmpty()) {
                //不为空
                MuxerData md = queue.remove(0);
                if(md == null){
                    continue;
                }
                if (md.isAudio) {
                    Log.e("xhc", "*audio time " + md.info.presentationTimeUs + " size " + md.info.size + " offset " + md.info.offset
                            + " capacity  " + md.byteBuffer.capacity());
                    muxer.writeSampleData(audioTrack, md.byteBuffer, md.info);
                } else {
                    Log.e("xhc", "*video time " + md.info.presentationTimeUs + " size " + md.info.size + " offset " + md.info.offset
                            + " capacity  " + md.byteBuffer.capacity());
                    muxer.writeSampleData(videoTrack, md.byteBuffer, md.info);
                }
            }

//            byte[] buffer = FFmpegUtils.getNextNalu(h264Path);
//            if(buffer != null && buffer.length > 0){
//
//                MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
//                ByteBuffer byteBuffer = ByteBuffer.allocate((int)(width * height * (float)3 / 2));
//                bufferInfo.size = buffer.length;
//                bufferInfo.offset = 0;
//                byte head = buffer[0];
//                int type = head & 0x1f;
//                bufferInfo.flags = 0;
//                if(type == 5){
//                    bufferInfo.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME;
//                }
//                bufferInfo.presentationTimeUs = System.currentTimeMillis() - startTime;
//                Log.e("xhc" , " time "+ bufferInfo.presentationTimeUs);
//                muxer.writeSampleData(videoTrack , byteBuffer , bufferInfo );
//
//            }
//            try {
//                Thread.sleep(40);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }

        }
    }

}
