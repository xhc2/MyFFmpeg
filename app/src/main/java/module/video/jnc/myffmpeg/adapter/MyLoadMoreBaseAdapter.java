package module.video.jnc.myffmpeg.adapter;


import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;

public abstract class MyLoadMoreBaseAdapter<T, V extends RecyclerView.ViewHolder> extends RecyclerView.Adapter<V> {

    protected List<T> list = new ArrayList<T>();
    protected Context context;
    protected MyBaseAdapter.OnRecyleItemClick lis;
    protected final int FOOTER_TYPE = 100;
    protected final int NORMAL_TYPE = 1;

    protected final int STATE_FINISH = 1;
    protected final int STATE_LOADING = 2;
    protected final int STATE_NOMORE = 3;
    protected int state = STATE_FINISH;
    public MyLoadMoreBaseAdapter(List<T> list, Context context) {
        this.context = context;
        if (list != null) {
            this.list.addAll(list);
        }
    }

    @Override
    public int getItemViewType(int position) {

        if (position >= list.size()) {
            return FOOTER_TYPE;
        }
        return NORMAL_TYPE;
    }

    @Override
    public int getItemCount() {
        return list.size() + 1; //footer type
    }

    public void setOnRecyleItemClick(module.video.jnc.myffmpeg.adapter.MyBaseAdapter.OnRecyleItemClick lis) {
        this.lis = lis;
    }

    public void refreshAllData(List<T> list) {
        this.list.clear();
        if (list != null) {
            this.list.addAll(list);
        }
        this.notifyDataSetChanged();
    }

    public void addAllItem(List<T> list){

        this.list.addAll(list);
//        notifyItemChanged();
        notifyItemRangeChanged(this.list.size() - list.size() - 1, list.size());
    }

    public void refreshItem(T t, int postion) {
        list.set(postion, t);
        notifyItemChanged(postion);
    }

    public void addItem(T t, int postion) {
        list.add(t);
        notifyItemInserted(postion);
    }

    public void removeItem(T t, int postion) {
        list.remove(t);
        notifyItemRemoved(postion);
    }


    @Override
    public abstract V onCreateViewHolder(ViewGroup container, int type);

    @Override
    public abstract void onBindViewHolder(V holder, int position);


    public void loadFinish(){
        state = STATE_FINISH;
        notifyItemChanged(getItemCount() - 1);
    }
    public void loadStart(){
        state = STATE_LOADING;
        notifyItemChanged(getItemCount() - 1);
    }

    public void loadNoMore(){
        state = STATE_NOMORE;
        notifyItemChanged(getItemCount() - 1);
    }

    public class FooterViewHolder extends RecyclerView.ViewHolder {
        ProgressBar loadBar;
        TextView tvText;

        public FooterViewHolder(View itemView) {
            super(itemView);
            loadBar = itemView.findViewById(R.id.progress_bar);
            tvText = itemView.findViewById(R.id.tv_text);
        }

        public void loadStart() {
            Log.e("xhc", " load more ");
            tvText.setVisibility(View.VISIBLE);
            loadBar.setVisibility(View.VISIBLE);
            tvText.setText("正在加载...");
        }

        public void loadFinish() {
            Log.e("xhc", " loadFinish ");
            tvText.setVisibility(View.GONE);
            loadBar.setVisibility(View.GONE);
        }

        public void loadNoMore() {
            Log.e("xhc", " noMoreData ");
            tvText.setVisibility(View.VISIBLE);
            loadBar.setVisibility(View.GONE);
            tvText.setText("已加载全部");
        }

    }


}
