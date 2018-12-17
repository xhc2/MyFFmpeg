package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.PlayListAdapter;
import module.video.jnc.myffmpeg.tool.Constant;

public class PlayListActivity extends BaseActivity {
    private RecyclerView rcView;
    private List<String> listFile = new ArrayList<>();
    private PlayListAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_list);
        rcView = findViewById(R.id.rv_view);
        init();
    }



    private void init(){
        getFileList();
        adapter = new PlayListAdapter(listFile , PlayListActivity.this);
        rcView.setLayoutManager(new LinearLayoutManager(PlayListActivity.this));
        rcView.addItemDecoration(new DividerItemDecoration(PlayListActivity.this, DividerItemDecoration.VERTICAL));
        rcView.setAdapter(adapter);
        adapter.setOnRecyleItemClick(new MyBaseAdapter.OnRecyleItemClick() {
            @Override
            public void onItemClick(View v, Object o, int position) {
                Intent intent = new Intent(PlayListActivity.this , Mp4PlayerActivity.class);
                intent.putExtra("path" , listFile.get(position));
                startActivity(intent);
            }
        });
    }

    private void getFileList() {
        File[] files = Constant.rootVideoFile.listFiles();
        if (files != null) {
            for (File f : files) {
                listFile.add(f.getPath());
            }
        }
        listFile.add(0 , "rtmp://live.hkstv.hk.lxdns.com/live/hks");
    }
}
