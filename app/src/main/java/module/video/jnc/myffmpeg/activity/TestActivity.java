package module.video.jnc.myffmpeg.activity;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyLoadMoreBaseAdapter;
import module.video.jnc.myffmpeg.widget.MyLoadMoreRecycleView;

public class TestActivity extends AppCompatActivity {

    private MyLoadMoreRecycleView recyclerView;
    private MyAdapter adapter;
    private List<Test> list = new ArrayList<>();
    int count = 100 ;
    int loadCount = 10 ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test2);

        recyclerView = findViewById(R.id.recycler_view);
        for (int i = 0; i < 10; ++i) {
            Test t = new Test();
            t.name = "xhc";
            list.add(t);
        }
        adapter = new MyAdapter(list, this);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);


        recyclerView.setLis(new MyLoadMoreRecycleView.LoadMoreListener() {
            @Override
            public void loadMore() {
                if( loadCount >= count ){
                    recyclerView.loadNoMore();
                    return ;
                }
                recyclerView.loadStart();
                new MyNet().start();

            }
        });
    }

    Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    Log.e("xhc", " load finish ");
                    adapter.addAllItem(list);
                    recyclerView.loadFinish();
                    break;

                case 3:
                    recyclerView.loadNoMore();
                    break;
            }
            return false;
        }
    });

    class MyNet extends Thread {
        @Override
        public void run() {
            super.run();
            try {
                list.clear();
                for (int i = 0; i < 10; ++i) {
                    Test t = new Test();
                    t.name = "MyNet";
                    list.add(t);
                }
                loadCount +=10;
                Thread.sleep(1000);
                handler.sendEmptyMessage(1);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    public class MyAdapter extends MyLoadMoreBaseAdapter<Test, RecyclerView.ViewHolder> {

        public MyAdapter(List<Test> list, Context context) {
            super(list, context);
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup container, int type) {
            switch (type) {
                case NORMAL_TYPE:
                    return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.player_list_layout_item, container, false));

                case FOOTER_TYPE:

                    return new FooterViewHolder(LayoutInflater.from(context).inflate(R.layout.item_footer_view, container, false));

            }
            return new ViewHolder(LayoutInflater.from(context).inflate(R.layout.player_list_layout_item, container, false));
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
            if (holder.getItemViewType() == NORMAL_TYPE) {
                ViewHolder normalHolder = (ViewHolder) holder;
                normalHolder.tv.setText(list.get(position).getName());
            } else {
                FooterViewHolder footerViewHolder = (FooterViewHolder) holder;
                switch (state) {
                    case STATE_FINISH:
                        footerViewHolder.loadFinish();
                        break;
                    case STATE_LOADING:
                        footerViewHolder.loadStart();
                        break;
                    case STATE_NOMORE:
                        footerViewHolder.loadNoMore();
                        break;
                }

            }
        }

        class ViewHolder extends RecyclerView.ViewHolder {
            ImageView img;
            TextView tv;
            RelativeLayout rlRoot;

            ViewHolder(View itemView) {
                super(itemView);
                rlRoot = itemView.findViewById(R.id.rl_root);
                img = itemView.findViewById(R.id.img_logo);
                tv = itemView.findViewById(R.id.tv_name);
            }
        }

    }


    class Test {
        private String name;

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }
    }

}
