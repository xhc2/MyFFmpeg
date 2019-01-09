package module.video.jnc.myffmpeg.adapter;

import android.content.Context;
import android.net.Uri;
import android.os.Trace;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.bumptech.glide.Glide;

import java.io.File;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.bean.FileBean;

public class ChoiseVideoAdapter extends MyBaseAdapter<FileBean, ChoiseVideoAdapter.ViewHolder> {

    LayoutInflater inflater;

    public ChoiseVideoAdapter(List<FileBean> list, Context context) {
        super(list, context);
        inflater = LayoutInflater.from(context);
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup container, int type) {
        return new ViewHolder(inflater.inflate(R.layout.file_item, container, false));
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, final int position) {
//        Trace.beginSection(" choise_video_adapter_onBindViewHolder");
        final FileBean fileBean = list.get(position);
        holder.imgVideo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (lis != null) {
                    lis.onItemClick(v, fileBean, position);
                }
            }
        });

        Glide.with(context).load(fileBean.getChoiseRid()).into(holder.imgChoise);
        Glide.with(context).load(Uri.fromFile(new File(fileBean.getPath()))).into(holder.imgVideo);
//        Trace.endSection();
    }

    private String getFileName(String name) {
        String[] strs = name.split("/");
        return strs[strs.length - 1];
    }

    class ViewHolder extends RecyclerView.ViewHolder {
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
