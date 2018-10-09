package module.video.jnc.myffmpeg.MediaCodec;

import android.media.MediaCodec;

import java.nio.ByteBuffer;

public class MuxerData {
    public ByteBuffer byteBuffer;
    public MediaCodec.BufferInfo info;
    public boolean isAudio;
}
