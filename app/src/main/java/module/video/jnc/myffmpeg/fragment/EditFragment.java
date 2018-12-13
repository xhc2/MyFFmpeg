package module.video.jnc.myffmpeg.fragment;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.activity.ChoisePicActivity;
import module.video.jnc.myffmpeg.activity.ChoiseVideoActivity;

public class EditFragment extends Fragment implements View.OnClickListener {

    private ImageView imgBmp;

    public EditFragment() {
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.activity_video_edit, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        view.findViewById(R.id.tv_video_edit).setOnClickListener(this);
        view.findViewById(R.id.tv_video_joint).setOnClickListener(this);
        view.findViewById(R.id.tv_watermark).setOnClickListener(this);
        view.findViewById(R.id.tv_crop).setOnClickListener(this);
        view.findViewById(R.id.tv_spcial_filter).setOnClickListener(this);
        view.findViewById(R.id.tv_scale).setOnClickListener(this);
        view.findViewById(R.id.tv_gif).setOnClickListener(this);
        view.findViewById(R.id.tv_reverse).setOnClickListener(this);
        view.findViewById(R.id.tv_dub).setOnClickListener(this);
        imgBmp = view.findViewById(R.id.img_bmp);
    }

    @Override
    public void onClick(View v) {
        Intent intent = new Intent();

        switch (v.getId()) {
            case R.id.tv_video_edit:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.clip");
                startActivity(intent);
                break;
            case R.id.tv_video_joint:
                //视频拼接
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 3);
                intent.putExtra("choise_min_video", 2);
                intent.putExtra("action", "xhc.video.joint");
                startActivity(intent);
                break;
            case R.id.tv_watermark:
                intent.setClass(getActivity(), ChoisePicActivity.class);
                intent.putExtra("choise_max_num_pic", 1);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.water_mark");
                intent.putExtra("action_next", "module.video.jnc.myffmpeg.choisevideo");
                startActivity(intent);
                break;
            case R.id.tv_crop:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.crop");
                startActivity(intent);
                break;
            case R.id.tv_spcial_filter:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.spcial_filter");
                startActivity(intent);
                break;
            case R.id.tv_scale:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.scale");
                startActivity(intent);
                break;
            case R.id.tv_gif:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.gif");
                startActivity(intent);
                break;
            case R.id.tv_reverse:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.reverse");
                startActivity(intent);
                break;
            case R.id.tv_dub:
                intent.setClass(getActivity(), ChoiseVideoActivity.class);
                intent.putExtra("choise_max_video", 1);
                intent.putExtra("action", "xhc.video.dub");
                startActivity(intent);
                break;

        }
    }
}
