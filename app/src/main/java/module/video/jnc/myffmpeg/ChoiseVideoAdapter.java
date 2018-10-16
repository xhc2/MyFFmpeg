package module.video.jnc.myffmpeg;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import java.util.List;

import module.video.jnc.myffmpeg.bean.FileBean;

public class ChoiseVideoAdapter extends MyBaseAdapter<FileBean, ChoiseVideoAdapter.ViewHolder>{

    public ChoiseVideoAdapter(List<FileBean> list, Context context) {
        super(list, context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.file_item , container , false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, final int position) {
        final FileBean fileBean =  list.get(position);
        holder.imgVideo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(lis != null){
                    lis.onItemClick(v , fileBean , position);
                }
            }
        });
        if(fileBean.isChoise()){
            holder.imgChoise.setImageResource(R.mipmap.choise);
        }
        else{
            holder.imgChoise.setImageDrawable(null);
        }
        displayImageView(holder.imgVideo , "file:///mnt"+fileBean.getPath());

    }

    class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imgVideo;
        ImageView imgChoise;
        public ViewHolder(View itemView) {
            super(itemView);
            imgVideo = itemView.findViewById(R.id.img_video);
            imgChoise = itemView.findViewById(R.id.img_choise);
        }
    }

}
