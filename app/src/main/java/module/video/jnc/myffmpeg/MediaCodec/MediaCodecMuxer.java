package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;

import java.io.IOException;

/**
 * 混合编码
 */
public class MediaCodecMuxer {

    public static final String MIMETYPE_VIDEO_AVC = "video/avc";
    public static final String MIMETYPE_AUDIO_AAC = "audio/mp4a-latm";
    private  MediaMuxer mMediaMuxer;
    private  MediaFormat mfVideo;
    private MediaFormat mfAudio;
    public MediaCodecMuxer(String path){
        try {
            mMediaMuxer = new MediaMuxer(path ,
                    MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public MediaCodecMuxer buildVideoFormat(int width , int height ){
        mfVideo = MediaFormat.createVideoFormat(MIMETYPE_VIDEO_AVC , width , height );
        return this;
    }

    public MediaCodecMuxer buildVideoBitRate(int bitRate){
        mfVideo.setInteger(MediaFormat.KEY_BIT_RATE , bitRate);
        return this;
    }

    public MediaCodecMuxer buildVideoFrameInter(int interSecond){
        mfVideo.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, interSecond);
        return this;
    }

    public MediaCodecMuxer buildVideoFrameRate(int frameRate){
        mfVideo.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
        return this;
    }

    //MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar
    public MediaCodecMuxer buildVideoColorFormat(int format){
        mfVideo.setInteger(MediaFormat.KEY_COLOR_FORMAT, format);
        return this;
    }

    public MediaCodecMuxer buildAudioFormat(int sampleRate , int channelCount){
        mfAudio =  MediaFormat.createAudioFormat(MIMETYPE_AUDIO_AAC , sampleRate , channelCount);
        return this;
    }

    public MediaCodecMuxer buildAudioBitRate(int bitRate){
        mfAudio.setInteger(MediaFormat.KEY_BIT_RATE , bitRate);
        return this;
    }

    public MediaCodecMuxer buildAudioProfile(int profile){
        mfAudio.setInteger(MediaFormat.KEY_AAC_PROFILE , profile);
        return this;
    }

    public MediaCodecMuxer build(){
        mMediaMuxer.addTrack(mfVideo);
        mMediaMuxer.addTrack(mfAudio);
        return this;
    }

    public void start(){
        mMediaMuxer.start();
    }


    public void stop(){
        mMediaMuxer.stop();
        mMediaMuxer.release();
    }

}
