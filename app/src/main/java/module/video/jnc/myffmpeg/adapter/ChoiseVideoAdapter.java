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
        holder.tvName.setText(getFileName(fileBean.getPath()));//这里字符串拼接是个耗时操作。导致帧率严重下降
        displayImageView(holder.imgVideo , "file:///mnt"+fileBean.getPath());

    }
    private String getFileName(String name){
        String[] strs = name.split("/");
        return strs[strs.length - 1];
    }
    class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imgVideo;
        ImageView imgChoise;
        TextView tvName;
        public ViewHolder(View itemView) {
            super(itemView);
            imgVideo = itemView.findViewById(R.id.img_video);
            imgChoise = itemView.findViewById(R.id.img_choise);
            tvName = itemView.findViewById(R.id.tv_name);
        }
    }

}
