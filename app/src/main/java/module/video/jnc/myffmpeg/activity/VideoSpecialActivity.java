package module.video.jnc.myffmpeg.activity;

import android.content.Intent;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.ChoiseSpecialFilterAdapter;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.bean.FilterBean;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

/**
 * 做视频颜色相关的滤镜
 * 好像gpuImagefilter 就是相关的滤镜
 */
public class VideoSpecialActivity extends SingleFilterActivity implements MyBaseAdapter.OnRecyleItemClick<FilterBean> {

    private List<FilterBean> list = new ArrayList<>();
    private RecyclerView rcChoiseView;
    private ChoiseSpecialFilterAdapter adapter;
    private FilterBean choiseFilterBean;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_special);
        findViewById();
        init();
    }

    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        rcChoiseView = findViewById(R.id.rl_choise);
    }

    protected void init() {
        super.init();
        initFilter();
        rcChoiseView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false));
        adapter = new ChoiseSpecialFilterAdapter(list, this);
        adapter.setOnRecyleItemClick(this);
        rcChoiseView.setAdapter(adapter);

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {

            @Override
            public void clickRight() {
                //右键点击
                FileUtils.makeFilterDir();
                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
                if (choiseFilterBean == null) {
                    showToast("请选择滤镜！");
                    return;
                }
                startDealVideo(listPath.get(0),
                        FileUtils.APP_FILTER + "filter" + System.currentTimeMillis() + ".mp4", choiseFilterBean.getFilterDes(), new int[]{-1, -1});
                showLoadPorgressDialog("处理中...");
            }
        });
    }


    private void initFilter() {
        FilterBean fbHeibai = new FilterBean(R.mipmap.heibai, "lutyuv='u=128:v=128'", "黑白");
        FilterBean fbMohu = new FilterBean(R.mipmap.mohu, "boxblur", "模糊");
        FilterBean fbMose = new FilterBean(R.mipmap.mose, "colorchannelmixer=.393:.769:.189:0:.349:.686:.168:0:.272:.534:.131", "墨色");
        FilterBean fbNongYu = new FilterBean(R.mipmap.nongyu, "hue='h=60:s=-3'", "浓郁");
        FilterBean fbYouhua = new FilterBean(R.mipmap.youhua, "edgedetect=mode=colormix:high=0", "油画");
        FilterBean fbZaodian = new FilterBean(R.mipmap.zaodian, "noise=alls=20:allf=t+p", "噪点");

        list.add(fbHeibai);
        list.add(fbMohu);
        list.add(fbMose);
        list.add(fbNongYu);
        list.add(fbYouhua);
        list.add(fbZaodian);
    }


    @Override
    public void onItemClick(View v, FilterBean filterBean, int position) {
        choiseFilterBean = filterBean;
        for (FilterBean fb : list) {
            fb.setChoiseFlag(false);
        }
        choiseFilterBean.setChoiseFlag(true);
        adapter.refreshAllData(list);
    }
}



