package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.bean.FileBean;

public class ChoiseMusicAdapter extends MyBaseAdapter<FileBean , ChoiseMusicAdapter.ViewHolder> {

    public interface MusicPlayClickInter{
        void musicClick(FileBean fb , boolean oper);
    }

    private MusicPlayClickInter musicPlayClickInter;

    public void setMusicPlayClickInter(MusicPlayClickInter musicPlayClickInter) {
        this.musicPlayClickInter = musicPlayClickInter;
    }

    public ChoiseMusicAdapter(List<FileBean> list, Context context) {
        super(list, context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.music_layout_item , container , false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder,final int position) {
        final FileBean fb = list.get(position);
        holder.tvName.setText(getFileName(fb.getPath()));
        holder.rlRoot.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(lis != null){
                    lis.onItemClick(v ,fb , position);
                }
            }
        });
        if(fb.isMusicPlay()){
            holder.imgOper.setImageResource(R.mipmap.start);
        }
        else{
            holder.imgOper.setImageResource(R.mipmap.pause);
        }
        holder.imgOper.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(musicPlayClickInter != null){
                    musicPlayClickInter.musicClick(fb , !fb.isMusicPlay());
//                    fb.setMusicPlay(!fb.isMusicPlay());
//                    musicPlayClickInter.musicClick(fb , fb.isMusicPlay());
//                    notifyItemChanged(position);
                }
            }
        });
    }

    private String getFileName(String name){
        String[] strs = name.split("/");
        return strs[strs.length - 1];
    }


    class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imgOper;
        TextView tvName ;
        RelativeLayout rlRoot;
        public ViewHolder(View itemView) {
            super(itemView);
            imgOper = itemView.findViewById(R.id.img_oper);
            tvName = itemView.findViewById(R.id.tv_name);
            rlRoot = itemView.findViewById(R.id.rl_root);
        }
    }
}
