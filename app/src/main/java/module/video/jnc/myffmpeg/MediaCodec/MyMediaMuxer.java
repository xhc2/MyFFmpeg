package module.video.jnc.myffmpeg.MediaCodec;

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

    private String path;
    private boolean flag;
    private MediaMuxer muxer;
    private int videoTrack;
    private int audioTrack;
    private List<MuxerData> queue = new ArrayList<>();
    private boolean startFlag ;


    public MyMediaMuxer(String outpath) {
        this.path = outpath;
        try {
            muxer = new MediaMuxer(path, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException e) {
            Log.e("xhc", " excepton " + e.getMessage());
            e.printStackTrace();
        }
    }



    public void addAudioTrack(MediaFormat format){
        if(format != null){
            audioTrack = muxer.addTrack(format);
        }
    }

    public void addVideoTrack(MediaFormat format){
        if(format != null ){
            videoTrack = muxer.addTrack(format);
        }
    }

    private int count = 0;

    public synchronized void startMuxer() {
        if(startFlag){
            return ;
        }


        if(count == 1 ){
            startFlag = true;
            muxer.start();
            flag = true;
            this.start();
        }
        count ++ ;
    }

    public void stopMuxer() {
        flag = false;
        try {
            this.join();
            muxer.stop();
            muxer.release();
            muxer = null;
        } catch (Exception e) {
            Log.e("xhc", " muxer Exception "+e.getMessage());
            e.printStackTrace();
        }
        finally {
            startFlag = false;
        }
        Log.e("xhc", " muxer release ");
    }

    public void writeVideoData(ByteBuffer buffer ,  MediaCodec.BufferInfo info){
        MuxerData md = new MuxerData();
        md.isAudio = false;
        md.byteBuffer = buffer;
        md.info = info;
        queue.add(md);
    }

    public void writeAudioData(ByteBuffer buffer ,  MediaCodec.BufferInfo info){
        MuxerData md = new MuxerData();
        md.isAudio = true;
        md.byteBuffer = buffer;
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
                    Log.e("xhc" ," audio time "+md.info.presentationTimeUs);
                    muxer.writeSampleData(audioTrack, md.byteBuffer, md.info);
                } else {
                    Log.e("xhc" ," video time "+md.info.presentationTimeUs);
                    muxer.writeSampleData(videoTrack, md.byteBuffer, md.info);
                }
            }
        }
    }

}
