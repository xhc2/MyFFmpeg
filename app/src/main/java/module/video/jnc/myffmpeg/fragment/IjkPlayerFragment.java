package module.video.jnc.myffmpeg.fragment;

import android.graphics.SurfaceTexture;
import android.media.AudioManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewCompat;
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
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.bumptech.glide.Glide;

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
    private LinearLayoutManager linearLayoutManager;
    private FrameLayout contain;
    private FrameLayout videoViewContain;
    private ProgressBar loadingPro;
    private ImageView img ;
    private int currentPosition = 0;
    private boolean autoPlay = true;
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
        contain =  view.findViewById(R.id.fl_contain) ;
        videoViewContain = view.findViewById(R.id.video_contain);
        loadingPro = view.findViewById(R.id.loading);
        img = view.findViewById(R.id.img);
        recyclerView = view.findViewById(R.id.recycler_view);
        initVideoView();
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
            }

            @Override
            public void onScrolled(RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);
                linearLayoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
//                if(currentPosition  != lastPosition && (lastPosition >= linearLayoutManager.findFirstVisibleItemPosition()
//                        && lastPosition <= linearLayoutManager.findLastVisibleItemPosition())){
//                    View view = linearLayoutManager.findViewByPosition(lastPosition);
//                    if(view != null){
//                        view.findViewById(R.id.bt_start).setVisibility(View.VISIBLE);
//                        view.findViewById(R.id.img_view).setVisibility(View.VISIBLE);
//                    }
//                }
                if(currentPosition >= 0 && currentPosition < adapter.getItemCount()){
                    /**
                     * 1.正在播放的视频已经滑出去，a.自动播放下一条,autoPlay = true b.播放器停止autoPlay = false
                     * 2.正在播放的视频还在可视区域 a.继续播放
                     */
                    if(currentPosition < linearLayoutManager.findFirstVisibleItemPosition() && autoPlay){
                        //自动播放下一条
                        lastPosition = currentPosition;
                        currentPosition = linearLayoutManager.findFirstVisibleItemPosition();
                        playVideo(currentPosition , listVB.get(currentPosition));
                    }
                    else if(currentPosition > linearLayoutManager.findLastVisibleItemPosition() && autoPlay){
                        lastPosition = currentPosition;
                        currentPosition = linearLayoutManager.findLastVisibleItemPosition();
                        playVideo(currentPosition , listVB.get(currentPosition));
                    }
                    else if((currentPosition < linearLayoutManager.findFirstVisibleItemPosition() ||
                            currentPosition > linearLayoutManager.findLastVisibleItemPosition()) && !autoPlay){
                        //没有自动播放，如果超出了可视区域直接停止播放器
                        new Thread(){
                            @Override
                            public void run() {
                                super.run();
                                mv.playRelease();
                            }
                        }.start();
                        return ;
                    }
                    contain.setTranslationY(linearLayoutManager.findViewByPosition(currentPosition).getY());
                }
            }
        });
    }

    private void addFile() {
        VideoBean vb = new VideoBean();
        vb.setVideoPath("sdcard/FFmpeg/video_src/v1080.mp4");

        VideoBean vb2 = new VideoBean();
        vb2.setVideoPath("sdcard/FFmpeg/video_src/test.mp4");

        VideoBean vb3 = new VideoBean();
        vb3.setVideoPath("sdcard/FFmpeg/video_src/time.mp4");

        VideoBean vb4 = new VideoBean();
        vb4.setVideoPath("sdcard/FFmpeg/video_src/input.mp4");

        VideoBean vb5 = new VideoBean();
        vb5.setVideoPath("rtmp://58.200.131.2:1935/livetv/hunantv");

        VideoBean vb6 = new VideoBean();
        vb6.setVideoPath("rtmp://media3.sinovision.net:1935/live/livestream");
        listVB.add(vb);
        listVB.add(vb);
        listVB.add(vb);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb2);
        listVB.add(vb3);
        listVB.add(vb3);
        listVB.add(vb4);
        listVB.add(vb5);
        listVB.add(vb6);
    }

    private MyVideoView mv;
    private void initVideoView(){
        mv = new MyVideoView(getContext());
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT);
        videoViewContain.addView(mv,params);
    }

    private int lastPosition = -1;
    private void playVideo(final int position, VideoBean videoBean) {


//        adapter.notifyPlayState(position , true);
        contain.setVisibility(View.VISIBLE);
        loadingPro.setVisibility(View.VISIBLE);
        img.setVisibility(View.VISIBLE);
        Glide.with(getContext()).load("http://b.hiphotos.baidu.com/image/pic/item/03087bf40ad162d93b3a196f1fdfa9ec8b13cde9.jpg").into(img);
        mv.play(videoBean.getVideoPath() ,"tag" , new MyVideoView.StartSuccessInterface(){
            @Override
            public void startSuccess() {
                Log.e("xhc" , " start success ");
                loadingPro.setVisibility(View.GONE);
                img.setVisibility(View.GONE);
            }
        });
    }


    @Override
    public void onItemClick(View v, VideoBean videoBean, int position) {
        lastPosition = currentPosition;
        currentPosition = position;
        linearLayoutManager = (LinearLayoutManager) recyclerView.getLayoutManager();
        if(currentPosition >= linearLayoutManager.findFirstVisibleItemPosition()
                || currentPosition <= linearLayoutManager.findLastVisibleItemPosition()){
            contain.setTranslationY(linearLayoutManager.findViewByPosition(currentPosition).getY());
        }
        playVideo(position, videoBean);
    }
}
