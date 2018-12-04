package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import module.video.jnc.myffmpeg.R;

//"lutyuv='u=128:v=128'"; 黑白
//boxblur 模糊
// hue='h=60:s=-3' 色彩配置
//hflip 锐化
//colorchannelmixer=.393:.769:.189:0:.349:.686:.168:0:.272:.534:.131 墨色
//edgedetect=mode=colormix:high=0 //绘制边缘，有油画的感觉
/**
 *  做视频颜色相关的滤镜
 *  好像gpuImagefilter 就是相关的滤镜
 */
public class VideoSpecialActivity extends SingleFilterActivity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_special);
    }
}
