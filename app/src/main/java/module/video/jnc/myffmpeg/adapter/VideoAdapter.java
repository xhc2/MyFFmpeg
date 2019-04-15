package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.media.AudioManager;
import android.os.Trace;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;

import com.bumptech.glide.Glide;

import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.bean.VideoBean;
import module.video.jnc.myffmpeg.widget.MyGlSurfaceViewCommon;
import module.video.jnc.myffmpeg.widget.MyVideoView;
import tv.danmaku.ijk.media.player.IMediaPlayer;
import tv.danmaku.ijk.media.player.IjkMediaPlayer;

public class VideoAdapter extends MyBaseAdapter<VideoBean,VideoAdapter.MyViewHolder> {

    private LayoutInflater inflater ;
    private int playPostion = -1;
    private IjkMediaPlayer player;

    public VideoAdapter(List<VideoBean> list, Context context) {
        super(list, context);
        inflater = LayoutInflater.from(context);
    }

    public void notifyPlayState(int position , boolean state){
        playPostion = state ?  position :  -1;
    }

    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new MyViewHolder(inflater.inflate(R.layout.video_item_layout , container , false ));
    }

    @Override
    public void onBindViewHolder(MyViewHolder holder, int position) {
        holder.btStart.setTag(position);
        Log.e("xhc" , " bind "+position );
//        if(playPostion == position){
//            holder.btStart.setVisibility(View.GONE);
//            holder.imgView.setVisibility(View.GONE);
//        }
//        else{
            holder.btStart.setVisibility(View.VISIBLE);
            holder.imgView.setVisibility(View.VISIBLE);
            Glide.with(context).load("http://b.hiphotos.baidu.com/image/pic/item/03087bf40ad162d93b3a196f1fdfa9ec8b13cde9.jpg").into(holder.imgView);
//        }
    }


    class MyViewHolder extends RecyclerView.ViewHolder{
        ImageButton btStart;
        ImageView imgView;
        MyViewHolder(View itemView) {
            super(itemView);
            btStart = itemView.findViewById(R.id.bt_start);
            imgView = itemView.findViewById(R.id.img_view);
            btStart.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    int position = (int)v.getTag();
                    if(lis != null){
                        lis.onItemClick(v , list.get(position) , position);
                    }
//                    playPostion = position;
//                    notifyPlayState(position , true);
                }
            });
        }
    }

}
