package module.video.jnc.myffmpeg.fragment;

import android.media.AudioManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.VideoAdapter;
import module.video.jnc.myffmpeg.bean.VideoBean;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

import static android.support.v7.widget.RecyclerView.SCROLL_STATE_IDLE;
import static android.support.v7.widget.RecyclerView.SCROLL_STATE_SETTLING;

/**
 * 列表播放视频
 */

public class IjkPlayerFragment extends Fragment implements SurfaceHolder.Callback {

    static {
        IjkMediaPlayer.loadLibrariesOnce(null);
        IjkMediaPlayer.native_profileBegin("libijkplayer.so");
    }


    private IjkMediaPlayer player;
    private RecyclerView recyclerView;
    private VideoAdapter adapter;
    private List<VideoBean> listVB = new ArrayList<>();
    private int lastPlayPosition = -1;


    public IjkPlayerFragment() {

    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.ijkplayer_layout, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        recyclerView = view.findViewById(R.id.recycler_view);
        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity(), LinearLayoutManager.VERTICAL, false));
        addFile();
        adapter = new VideoAdapter(listVB, getActivity());
        recyclerView.setAdapter(adapter);
        recyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
                //只播放第一个item
                Log.e("xhc", " state " + newState);
                if (newState == SCROLL_STATE_IDLE) {
                    //停止且固定了
                    LinearLayoutManager l = (LinearLayoutManager) recyclerView.getLayoutManager();
                    int position = l.findFirstVisibleItemPosition();
                    if(position == lastPlayPosition){
                        return ;
                    }
                    Log.e("xhc" , " position "+position);
                    View view = l.getChildAt(position);
                    SurfaceView sv = view.findViewById(R.id.surface_view);
                    sv.getHolder().addCallback(IjkPlayerFragment.this);
                    try {
                        if(player != null && player.isPlaying()){
                            player.reset();
                            player.release();
                        }
                        player = new IjkMediaPlayer();
                        player.setAudioStreamType(AudioManager.STREAM_MUSIC);
                        player.setDisplay(sv.getHolder());
                        player.setDataSource(listVB.get(position).getVideoPath());
                        player.prepareAsync();
                        player.start();
                        lastPlayPosition = position;
                    } catch (Exception e) {
                        Log.e("xhc" , " "+e.getMessage());
                        e.printStackTrace();
                    }
                }
            }

            @Override
            public void onScrolled(RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);
            }
        });


//

//
//
    }

    private void addFile() {
        VideoBean vb = new VideoBean();
        vb.setVideoPath("rtmp://58.200.131.2:1935/livetv/hunantv");
        VideoBean vb2 = new VideoBean();
        vb2.setVideoPath("sdcard/FFmpeg/video_src/test.mp4");

        listVB.add(vb);
        listVB.add(vb);
        listVB.add(vb);
        listVB.add(vb);
        listVB.add(vb2);
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {

        Log.e("xhc", " surfaceCreated ");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.e("xhc", " surfaceChanged ");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.e("xhc", " surfaceDestroyed ");
    }
}
