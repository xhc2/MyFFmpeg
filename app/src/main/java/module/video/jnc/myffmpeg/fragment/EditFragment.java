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
import module.video.jnc.myffmpeg.activity.ChoiseVideoActivity;

public class EditFragment extends Fragment implements View.OnClickListener{

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
        imgBmp = view.findViewById(R.id.img_bmp);
//        BitmapFactory.de
    }

    @Override
    public void onClick(View v) {
        Intent intent = new Intent();
        intent.setClass(getActivity() , ChoiseVideoActivity.class);
        switch (v.getId()){
            case R.id.tv_video_edit:
                intent.putExtra("choise_num" , 1);
                intent.putExtra("action" , "xhc.video.clip");
                startActivity(intent);
                break;
            case R.id.tv_video_joint:
                //视频拼接
                intent.putExtra("choise_num" , 3);
//                intent.putExtra("choise_min" , 2);
                intent.putExtra("action" , "xhc.video.joint");
                startActivity(intent);
                break;
            case R.id.tv_watermark:
                FFmpegUtils.initBitmapWaterMark("sdcard/FFmpeg/test.mp4" , "sdcard/FFmpeg/my_logo.png" , 10 , 10);
                FFmpegUtils.bitmapWaterMarkStart();
                break;
        }
    }
}
