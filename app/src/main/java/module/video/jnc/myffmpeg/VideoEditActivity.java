package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.os.Bundle;

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
public class VideoEditActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_edit);

    }
}









































