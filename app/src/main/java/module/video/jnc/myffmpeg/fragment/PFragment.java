package module.video.jnc.myffmpeg.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.activity.Mp4PlayerActivity;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.PlayListAdapter;
import module.video.jnc.myffmpeg.tool.Constant;

public class PFragment extends Fragment {

    private RecyclerView rcView;
    private List<String> listFile = new ArrayList<>();
    private PlayListAdapter adapter;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.frg_player_layout, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        rcView = view.findViewById(R.id.rv_view);
        init();
    }

    private void init(){
        getFileList();
        adapter = new PlayListAdapter(listFile , getActivity());
        rcView.setLayoutManager(new LinearLayoutManager(getActivity()));
        rcView.addItemDecoration(new DividerItemDecoration(getActivity(), DividerItemDecoration.VERTICAL));
        rcView.setAdapter(adapter);
        adapter.setOnRecyleItemClick(new MyBaseAdapter.OnRecyleItemClick() {
            @Override
            public void onItemClick(View v, Object o, int position) {
                Intent intent = new Intent(getActivity() , Mp4PlayerActivity.class);
                intent.putExtra("path" , listFile.get(position));
                startActivity(intent);
            }
        });
        Log.e("xhc" , "PlayFragment list ");
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
