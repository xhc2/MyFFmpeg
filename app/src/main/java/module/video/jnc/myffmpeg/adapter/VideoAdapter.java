package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.bean.VideoBean;

public class VideoAdapter extends MyBaseAdapter<VideoBean,VideoAdapter.MyViewHolder> {

    private LayoutInflater inflater ;

    public VideoAdapter(List<VideoBean> list, Context context) {
        super(list, context);
        inflater = LayoutInflater.from(context);
    }

    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new MyViewHolder(inflater.inflate(R.layout.video_item_layout , container , false ));
    }

    @Override
    public void onBindViewHolder(MyViewHolder holder, int position) {

    }

    class MyViewHolder extends RecyclerView.ViewHolder{
        SurfaceView surfaceView;
        Button btStart;
        MyViewHolder(View itemView) {
            super(itemView);
            surfaceView = itemView.findViewById(R.id.surface_view);
            btStart = itemView.findViewById(R.id.bt_start);
        }
    }

}
