package module.video.jnc.myffmpeg.fragment;

import android.graphics.SurfaceTexture;
import android.media.AudioManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.VideoAdapter;
import module.video.jnc.myffmpeg.bean.VideoBean;
import module.video.jnc.myffmpeg.widget.MyGlSurfaceViewCommon;
import module.video.jnc.myffmpeg.widget.MyVideoView;
import tv.danmaku.ijk.media.player.IMediaPlayer;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

import static android.support.v7.widget.RecyclerView.SCROLL_STATE_IDLE;
import static android.support.v7.widget.RecyclerView.SCROLL_STATE_SETTLING;

/**
 * 列表播放视频
 */

public class IjkPlayerFragment extends Fragment implements MyBaseAdapter.OnRecyleItemClick<VideoBean> {

    static {
        IjkMediaPlayer.loadLibrariesOnce(null);
        IjkMediaPlayer.native_profileBegin("libijkplayer.so");
    }


    private RecyclerView recyclerView;
    private VideoAdapter adapter;
    private List<VideoBean> listVB = new ArrayList<>();
    private int lastPlayPositon = -1;
    private boolean playState = false;
    private LinearLayoutManager linearLayoutManager;
    private View viewItem;
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
        adapter.setOnRecyleItemClick(this);
        recyclerView.setAdapter(adapter);
        recyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
                //只播放第一个item
//                Log.e("xhc", " state " + newState);
//                LinearLayoutManager l = (LinearLayoutManager) recyclerView.getLayoutManager();
//                if (lastPlayPositon != -1) {
//                    View view = l.getChildAt(lastPlayPositon);
//                    if (view.getY() < 0) {
//                        //item已经准备划出去 , 那么就播放下一个
//                        Log.e("xhc" , " view.gety "+view.getY());
//                    }
//
//                }


//                int position = l.findFirstVisibleItemPosition();
//                View view = l.getChildAt(position);
//                Log.e("xhc" , " item y"+ view.getY());
//

//
//                if (newState == SCROLL_STATE_IDLE) {
//                    if(position == lastPlayPosition){
//                        return ;
//                    }
//
//                    SurfaceView sv = view.findViewById(R.id.surface_view);
//                    sv.getHolder().addCallback(IjkPlayerFragment.this);
//                    try {
//                        play(listVB.get(position) , sv);
//                        lastPlayPosition = position;
//                    } catch (Exception e) {
//                        Log.e("xhc" , " "+e.getMessage());
//                        e.printStackTrace();
//                    }
//                }
            }

            @Override
            public void onScrolled(RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);
                linearLayoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
                if(lastPlayPositon != -1 ){
                    viewItem = linearLayoutManager.getChildAt(lastPlayPositon);
                    Log.e("xhc" , " get y "+viewItem.getY() +" lastposition "+lastPlayPositon+" view item "+viewItem);

                    if((viewItem.getY() + viewItem.getHeight() / 2) <= 0){

                        if(lastPlayPositon + 1 < adapter.getItemCount()
                                && !videoView.compareTag(String.valueOf(lastPlayPositon + 1))){
                            Log.e("xhc" , " play next one ");
                            playVideo(lastPlayPositon + 1 , listVB.get(lastPlayPositon + 1));
                        }
                    }
                }
            }
        });
    }


    //http://ips.ifeng.com/video19.ifeng.com/video09/2019/04/05/p14578618-102-008-095505.mp4?vid=ffbe0b09-3e47-45f1-ba87-feca5da77997&uid=1536051367692_j0o9i7856&from=v_Free&pver=vHTML5Player_v2.0.0&sver=&se=%E8%87%AA%E5%AA%92%E4%BD%93&cat=165-10134&ptype=165&platform=pc&sourceType=h5&dt=1554428963000&gid=QpIIgWoKNEZC&sign=d1b82d3940da0f3dc8508c32c118869c&tm=1554949359175
    private void addFile() {
        VideoBean vb = new VideoBean();
        vb.setVideoPath("rtmp://58.200.131.2:1935/livetv/hunantv");
        VideoBean vb2 = new VideoBean();
        vb2.setVideoPath("sdcard/FFmpeg/video_src/test.mp4");

//        listVB.add(vb);
//        listVB.add(vb);
//        listVB.add(vb);
//        listVB.add(vb);
//        listVB.add(vb);
//        listVB.add(vb);
//        listVB.add(vb);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
    }


    private MyVideoView videoView;

    private MyVideoView initVideoView() {
        if (videoView == null) {
            videoView = new MyVideoView(getContext());
            FrameLayout.LayoutParams params =
                    new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT,
                            FrameLayout.LayoutParams.MATCH_PARENT);
            videoView.setLayoutParams(params);
        }
        return videoView;
    }

    private void playVideo(final int position, VideoBean videoBean) {
        LinearLayoutManager l = (LinearLayoutManager) recyclerView.getLayoutManager();
        Log.e("xhc", " nowposition " + lastPlayPositon + " position " + position);
        if (lastPlayPositon != position && lastPlayPositon != -1) {
            ((ViewGroup) l.getChildAt(lastPlayPositon).findViewById(R.id.fl_contain)).removeAllViews();
        }

        View viewRoot = l.getChildAt(position);
        FrameLayout fl = viewRoot.findViewById(R.id.fl_contain);
        fl.addView(initVideoView());
        videoView.playRelease();
        videoView.play(videoBean.getVideoPath(), position + "", new MyVideoView.StartSuccessInterface() {
            @Override
            public void startSuccess() {
                lastPlayPositon = position;
            }
        });
    }


    @Override
    public void onItemClick(View v, VideoBean videoBean, int position) {
        playVideo(position, videoBean);
    }
}
