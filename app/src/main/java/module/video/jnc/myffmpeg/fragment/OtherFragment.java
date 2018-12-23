package module.video.jnc.myffmpeg.fragment;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.activity.CameraStreamActivity;
import module.video.jnc.myffmpeg.activity.FlvParseActivity;
import module.video.jnc.myffmpeg.activity.HardCodeActivity;
import module.video.jnc.myffmpeg.activity.Mp4PlayerActivity;
import module.video.jnc.myffmpeg.activity.NetStreamActivity;

public class OtherFragment extends Fragment implements View.OnClickListener {


    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.frg_other_layout, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.tv_network).setOnClickListener(this);
        view.findViewById(R.id.tv_hard_decode).setOnClickListener(this);
        view.findViewById(R.id.tv_file_parse).setOnClickListener(this);
        view.findViewById(R.id.tv_rtmp_upload).setOnClickListener(this);
        view.findViewById(R.id.tv_rtmp_downlaod).setOnClickListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_network:
                startActivity(new Intent(getActivity(), NetStreamActivity.class));
                break;
            case R.id.tv_hard_decode:
                startActivity(new Intent(getActivity(), HardCodeActivity.class));
                break;
            case R.id.tv_file_parse:
                startActivity(new Intent(getActivity(), FlvParseActivity.class));
                break;
            case R.id.tv_rtmp_upload:
                startActivity(new Intent(getActivity(), CameraStreamActivity.class));
                break;
            case R.id.tv_rtmp_downlaod:
                Intent intent = new Intent(getActivity(), Mp4PlayerActivity.class);
                intent.putExtra("path", "rtmp:192.168.0.11:1935:/live/live");
                startActivity(intent);
                break;

        }
    }
}
