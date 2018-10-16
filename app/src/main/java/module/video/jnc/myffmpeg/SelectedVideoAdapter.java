package module.video.jnc.myffmpeg;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import java.util.List;

public class SelectedVideoAdapter extends MyBaseAdapter<String , SelectedVideoAdapter.ViewHolder> {


    public SelectedVideoAdapter(List<String> list, Context context) {
        super(list, context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.file_item , container , false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
//        holder.imgVideo
        String path = list.get(position);
        displayImageView(holder.imgVideo , "file:///mnt"+path);
    }

    class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imgVideo;
        public ViewHolder(View itemView) {
            super(itemView);
            imgVideo = itemView.findViewById(R.id.img_video);
        }
    }
}
