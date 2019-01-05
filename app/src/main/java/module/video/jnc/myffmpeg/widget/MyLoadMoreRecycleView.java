package module.video.jnc.myffmpeg.widget;

import android.content.Context;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.AttributeSet;
import android.util.Log;

import module.video.jnc.myffmpeg.activity.TestActivity;
import module.video.jnc.myffmpeg.adapter.MyLoadMoreBaseAdapter;

public class MyLoadMoreRecycleView extends RecyclerView {
    private boolean isLoadMore = false;
    private LoadMoreListener lis ;

    public void setLis(LoadMoreListener lis) {
        this.lis = lis;
    }

    public MyLoadMoreRecycleView(Context context) {
        this(context, null);
    }

    public MyLoadMoreRecycleView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MyLoadMoreRecycleView(Context context, @Nullable AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
            }

            @Override
            public void onScrolled(RecyclerView recyclerView, int dx, int dy) {
                super.onScrolled(recyclerView, dx, dy);
                if (!isLoadMore && dy >= 0) {
                    int totalItemCount = recyclerView.getLayoutManager().getItemCount();
                    int lastVisibleItem = ((LinearLayoutManager) recyclerView.getLayoutManager()).findLastVisibleItemPosition();
                    Log.e("xhc" , " total "+totalItemCount+" lastVisibleItem "+lastVisibleItem);

                    if (lastVisibleItem + 2 >= totalItemCount) {//倒数第二个就开始加载
                        isLoadMore = true;

                        if(lis != null){
                            lis.loadMore();
                        }
                    }
                }
            }
        });
    }

    public void loadFinish(){
        isLoadMore = false;
        Log.e("xhc" , " recycle loadFinish");
        MyLoadMoreBaseAdapter adapter = (MyLoadMoreBaseAdapter)getAdapter();
        adapter.loadFinish();
    }

    public void loadStart(){
        Log.e("xhc" , " recycle loadStart");
        MyLoadMoreBaseAdapter adapter = (MyLoadMoreBaseAdapter)getAdapter();
        adapter.loadStart();
    }
    public void loadNoMore(){
        Log.e("xhc" , " recycle loadNoMore");
        MyLoadMoreBaseAdapter adapter = (MyLoadMoreBaseAdapter)getAdapter();
        adapter.loadNoMore();
    }



    public interface LoadMoreListener{
        void loadMore();
    }

}
