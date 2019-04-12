package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.media.AudioManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;

import tv.danmaku.ijk.media.player.IMediaPlayer;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

/**
 * 用来配合ijkplayer做一个播放器
 */
public class MyVideoView extends FrameLayout implements SurfaceHolder.Callback {

    private IjkMediaPlayer player = new IjkMediaPlayer();
    private SurfaceView surfaceView;

    public MyVideoView(@NonNull Context context) {
        this(context, null);
    }

    public MyVideoView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MyVideoView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {

        surfaceView = new SurfaceView(getContext());
        this.addView(surfaceView);
        surfaceView.getHolder().addCallback(MyVideoView.this);

    }

    private String tag;

    public boolean compareTag(String tag) {
        return !TextUtils.isEmpty(tag) && tag.equals(this.tag);
    }

    public void play(String path, String tag) {
        this.tag = tag;
        play(path);
    }

    public interface StartSuccessInterface {
        void startSuccess();
    }

    private StartSuccessInterface lis;

    public void play(String path, String tag, StartSuccessInterface lis) {
        this.lis = lis;
        play(path, tag);
    }

    public void play(final String path) {
        playRelease();
        if (player == null) {
            player = new IjkMediaPlayer();
        }
        surfaceView.getHolder().removeCallback(this);
        surfaceView.getHolder().addCallback(MyVideoView.this);
        new Thread() {
            @Override
            public void run() {
                super.run();
                try {


                    player.setAudioStreamType(AudioManager.STREAM_MUSIC);
                    player.setDataSource(path);
                    player.prepareAsync();

                    player.setOnPreparedListener(new IMediaPlayer.OnPreparedListener() {
                        @Override
                        public void onPrepared(IMediaPlayer iMediaPlayer) {
                            if (lis != null) lis.startSuccess();
                            iMediaPlayer.start();
                            Log.e("xhc", " start ...");
                        }
                    });

                } catch (Exception e) {
                    Log.e("xhc", " e message " + e.getMessage());
                    e.printStackTrace();
                }
            }
        }.start();
    }

    public void playRelease() {
        if (player != null) {
            player.reset();
            player.release();
        }
        player = null;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.e("xhc" , " surfaceCreated "+this);
        player.setDisplay(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
//        Log.e("xhc" , " surfaceChanged "+this);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.e("xhc" , " surfaceDestroyed "+this);
        playRelease();
    }
}
