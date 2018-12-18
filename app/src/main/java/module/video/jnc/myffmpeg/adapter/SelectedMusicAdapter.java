package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

import module.video.jnc.myffmpeg.R;

public class SelectedMusicAdapter extends MyBaseAdapter<String , SelectedMusicAdapter.ViewHolder> {


    public SelectedMusicAdapter(List<String> list, Context context) {
        super(list, context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.music_choise_item, container, false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        String path = list.get(position);
        holder.imgVideo.setImageResource(R.mipmap.music_logo);
        holder.tv.setText(getFileName(path));
    }

    private String getFileName(String name){
        String[] strs = name.split("/");
        return strs[strs.length - 1];
    }

    class ViewHolder extends RecyclerView.ViewHolder {
        ImageView imgVideo;
        TextView tv;
        public ViewHolder(View itemView) {
            super(itemView);
            imgVideo = itemView.findViewById(R.id.img_video);
            tv = itemView.findViewById(R.id.tv_name);
        }
    }

}