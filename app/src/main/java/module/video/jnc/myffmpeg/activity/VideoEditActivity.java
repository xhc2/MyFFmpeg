package module.video.jnc.myffmpeg.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import module.video.jnc.myffmpeg.R;

/**
 * 一：视频相关的编辑-》这个相对比较简单
 *  1.视频剪辑（时间剪辑）
 *  2.视频拼接
 *  3.视频裁剪（swscale）
 *  4.分辨率（swscale）
 *  5.改变视频的播放速度
 *二：效果相关
 *  1.视频滤镜 （可以把uv手动修改，然后做个简单的滤镜）
 *  2.视频主题 （一个大的gif完全覆盖到视频上）
 *  3.水印（文字水印，图片水印，gif水印）
 * 三：后期相关
 *  1.视频配音（同步配音，不知是否可行）
 *  2.视频配乐
 *  3.倒放
 */
public class VideoEditActivity extends Activity implements View.OnClickListener{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_edit);
        findViewById(R.id.tv_video_edit).setOnClickListener(this);
        findViewById(R.id.tv_video_joint).setOnClickListener(this);
    }


    @Override
    public void onClick(View v) {
        Intent intent = new Intent();
        intent.setClass(VideoEditActivity.this , ChoiseVideoActivity.class);
        switch (v.getId()){
            case R.id.tv_video_edit:
                intent.putExtra("choise_num" , 1);
                intent.putExtra("action" , "xhc.video.clip");
                startActivity(intent);
                break;
            case R.id.tv_video_joint:
                //视频拼接
                intent.putExtra("choise_num" , 3);
                intent.putExtra("action" , "xhc.video.joint");
                startActivity(intent);
                break;
        }
    }
}









































