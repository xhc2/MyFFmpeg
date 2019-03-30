package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.bumptech.glide.Glide;

import java.io.File;
import java.util.List;

import module.video.jnc.myffmpeg.R;

public class SelectedVideoAdapter extends MyBaseAdapter<String , SelectedVideoAdapter.ViewHolder> {
    private LayoutInflater inflater ;

    public SelectedVideoAdapter(List<String> list, Context context) {
        super(list, context);
        inflater = LayoutInflater.from(context) ;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(inflater.inflate(R.layout.file_item , container , false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        String path = list.get(position);
        Glide.with(context).load(Uri.fromFile(new File(path))).into(holder.imgVideo);
    }

    class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imgVideo;
        public ViewHolder(View itemView) {
            super(itemView);
            imgVideo = itemView.findViewById(R.id.img_video);
        }
    }
}
