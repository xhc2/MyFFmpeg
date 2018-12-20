package module.video.jnc.myffmpeg.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.ArrayMap;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.activity.AboutActivity;
import module.video.jnc.myffmpeg.activity.Mp4PlayerActivity;
import module.video.jnc.myffmpeg.activity.MyDealFileActivity;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.adapter.PlayListAdapter;
import module.video.jnc.myffmpeg.tool.Constant;
import module.video.jnc.myffmpeg.tool.FileUtils;

public class MyFragment extends Fragment implements View.OnClickListener {

//    private Map<String , String > mapFile = new ArrayMap<>();

    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.frg_player_layout, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.tv_myfile).setOnClickListener(this);
        view.findViewById(R.id.tv_about).setOnClickListener(this);
        init();
    }

    private void init(){

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.tv_myfile:
                startActivity(new Intent(getActivity() , MyDealFileActivity.class));
                break;
            case R.id.tv_about:
                startActivity(new Intent(getActivity() , AboutActivity.class));
                break;
        }
    }
}
