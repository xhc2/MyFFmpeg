package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.RelativeLayout;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import module.video.jnc.myffmpeg.MyVideoGpuShow;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.adapter.ChoiseScaleAdapter;
import module.video.jnc.myffmpeg.adapter.ChoiseSpecialFilterAdapter;
import module.video.jnc.myffmpeg.adapter.MyBaseAdapter;
import module.video.jnc.myffmpeg.bean.FilterBean;
import module.video.jnc.myffmpeg.bean.ScaleBean;
import module.video.jnc.myffmpeg.tool.FileUtils;
import module.video.jnc.myffmpeg.widget.TitleBar;

public class VideoScaleActivity extends SingleFilterActivity implements MyBaseAdapter.OnRecyleItemClick<ScaleBean> {

    private List<ScaleBean> list = new ArrayList<>();
    private RecyclerView rcChoiseView;
    private ChoiseScaleAdapter adapter;
    private ScaleBean choiseScaleBean;
    private RelativeLayout rlCustom;
    private EditText etWidth ;
    private EditText etHeight ;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_scale);
        findViewById();
        init();
    }


    private void findViewById() {
        titleBar = findViewById(R.id.title);
        myVideoGpuShow = (MyVideoGpuShow) findViewById(R.id.play_gl_surfaceview);
        rcChoiseView = findViewById(R.id.rl_choise);
        rlCustom = findViewById(R.id.rl_custom);
        etWidth = findViewById(R.id.et_width);
        etHeight = findViewById(R.id.et_height);
    }

    protected void init() {
        super.init();
        initScale();
        adapter = new ChoiseScaleAdapter(list ,this);
        rcChoiseView.setLayoutManager(new LinearLayoutManager(this, LinearLayoutManager.HORIZONTAL, false));
        adapter.setOnRecyleItemClick(this);
        rcChoiseView.setAdapter(adapter);

        titleBar.setRightClickInter(new TitleBar.RightClickInter() {

            @Override
            public void clickRight() {
                //右键点击
                FileUtils.makeScaleDir();
                if (dealFlag) {
                    showToast("正在处理中，请稍等");
                    showLoadPorgressDialog("处理中...");
                    return;
                }
                if(choiseScaleBean == null ){
                    showToast("请选择分辨率！");
                    return ;
                }
                if("自定义".equals( choiseScaleBean.getName())){
                    if( TextUtils.isEmpty(etWidth.getText())){
                        showToast("请输入宽度！");
                        return ;
                    }
                    if( TextUtils.isEmpty(etWidth.getText())){
                        showToast("请输入高度！");
                        return ;
                    }
                    int width = Integer.parseInt(etWidth.getText().toString())  ;
                    int height = Integer.parseInt(etHeight.getText().toString())  ;
                    choiseScaleBean.setWidth(width);
                    choiseScaleBean.setHeight(height);
                }

                String filterDes =  String.format(Locale.CHINESE , "scale=w=%d:h=%d" , choiseScaleBean.getWidth() , choiseScaleBean.getHeight());

                startDealVideo(listPath.get(0),
                        FileUtils.APP_SCALE + "filter" + System.currentTimeMillis() + ".mp4",  filterDes,
                        new int[]{choiseScaleBean.getWidth(), choiseScaleBean.getHeight()});
                showLoadPorgressDialog("处理中...");
            }
        });
    }


    private void initScale(){
        ScaleBean sb1080 = new ScaleBean("1080p" , 1920 , 1080);
        ScaleBean sb720 = new ScaleBean("720p" , 1280 , 720);
        ScaleBean sb540 = new ScaleBean("540p" , 960 , 540);
        ScaleBean sb480 = new ScaleBean("480p" , 640 , 480);
        ScaleBean sb360 = new ScaleBean("360p" , 480 , 360);
        ScaleBean sbOther = new ScaleBean("自定义" , 0 , 0);

        list.add(sb1080);
        list.add(sb720);
        list.add(sb540);
        list.add(sb480);
        list.add(sb360);
        list.add(sbOther);
    }

    @Override
    public void onItemClick(View v, ScaleBean scaleBean, int position) {
        choiseScaleBean = scaleBean;
        for (ScaleBean sb : list) {
            sb.setChoiseFlag(false);
        }
        choiseScaleBean.setChoiseFlag(true);
        adapter.refreshAllData(list);
        if("自定义".equals( choiseScaleBean.getName())){
            rlCustom.setVisibility(View.VISIBLE);
        }
        else{
            rlCustom.setVisibility(View.GONE);
        }
    }
}
