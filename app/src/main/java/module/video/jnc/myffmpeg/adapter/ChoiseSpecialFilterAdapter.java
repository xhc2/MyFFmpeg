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
import module.video.jnc.myffmpeg.bean.FilterBean;

public class ChoiseSpecialFilterAdapter extends MyBaseAdapter<FilterBean, ChoiseSpecialFilterAdapter.MyViewHolder> {

    public ChoiseSpecialFilterAdapter(List<FilterBean> list, Context context) {
        super(list, context);
    }

    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new MyViewHolder(LayoutInflater.from(context).inflate(R.layout.special_filter_item , container , false));
    }

    @Override
    public void onBindViewHolder(MyViewHolder holder, final int position) {
        final FilterBean fb = list.get(position);
        holder.imgVideo.setImageResource(fb.getLogoId());
        holder.tvName.setText(fb.getName());
        if(fb.isChoiseFlag()){
            holder.imgChoise.setImageResource(R.mipmap.choise);
        }
        else{
            holder.imgChoise.setImageDrawable(null);
        }
        holder.imgVideo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(lis != null){
                    lis.onItemClick(v , fb , position);
                }
            }
        });

    }

    class MyViewHolder extends RecyclerView.ViewHolder{
        ImageView imgVideo;
        TextView tvName;
        ImageView imgChoise;
        public MyViewHolder(View itemView) {
            super(itemView);
            imgChoise = itemView.findViewById(R.id.img_choise);
            imgVideo = itemView.findViewById(R.id.img_video);
            tvName = itemView.findViewById(R.id.tv_name);
        }
    }
}
