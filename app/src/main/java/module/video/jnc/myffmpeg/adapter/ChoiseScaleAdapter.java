package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.text.Layout;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.bean.ScaleBean;

public class ChoiseScaleAdapter extends MyBaseAdapter<ScaleBean ,ChoiseScaleAdapter.ViewHolder > {

    public ChoiseScaleAdapter(List<ScaleBean> list, Context context) {
        super(list, context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.scale_item , container , false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder,final int position) {
        final ScaleBean sb = list.get(position);
        holder.tvName.setText(sb.getName());
        holder.tvWH.setText(sb.getWidth()+"x"+sb.getHeight());
        if(sb.isChoiseFlag()){
            holder.imgChoise.setImageResource(R.mipmap.choise);
        }
        else{
            holder.imgChoise.setImageDrawable(null);
        }
        holder.tvName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(lis != null){
                    lis.onItemClick(v , sb , position);
                }
            }
        });
    }

    class ViewHolder extends RecyclerView.ViewHolder{
        TextView tvName;
        TextView tvWH;
        ImageView imgChoise;

        public ViewHolder(View itemView) {
            super(itemView);
            tvName = itemView.findViewById(R.id.tv_name);
            tvWH = itemView.findViewById(R.id.tv_width_height);
            imgChoise = itemView.findViewById(R.id.img_choise);

        }
    }

}
