package module.video.jnc.myffmpeg.widget;

import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.DensityUtils;

public class TitleBar extends RelativeLayout{

    private TextView tvTitle;
    private String title ;
    private String rightText;
    public TitleBar(Context context) {
        this(context , null);
    }

    public TitleBar(Context context, AttributeSet attrs) {
        this(context, attrs , 0);
    }

    public TitleBar(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        TypedArray array = context.obtainStyledAttributes(attrs, R.styleable.title_bar);
        title = array.getString(R.styleable.title_bar_title_text);
        rightText= array.getString(R.styleable.title_bar_right_text);
        array.recycle();
        init();
    }

    private void init(){
        this.setBackgroundColor(Color.parseColor("#2473f0"));
        tvTitle = new TextView(getContext());
        LayoutParams titleParams = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT , ViewGroup.LayoutParams.MATCH_PARENT);
        tvTitle.setLayoutParams(titleParams);
        titleParams.addRule(CENTER_IN_PARENT);
        tvTitle.setGravity(Gravity.CENTER);
        tvTitle.setTextSize(16);
        tvTitle.setTextColor(Color.parseColor("#ffffff"));
        if(!TextUtils.isEmpty(title)){
            tvTitle.setText(title);
        }

        this.addView(tvTitle);

        ImageView back = new ImageView(getContext());
        back.setImageResource(R.mipmap.back);
        LayoutParams backParams = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT , ViewGroup.LayoutParams.MATCH_PARENT);
        back.setLayoutParams(backParams);
        int dp5 = DensityUtils.dip2px(getContext() , 5);
        back.setPadding(dp5 , 0 , dp5 , 0);
        back.setImageResource(R.mipmap.back);
        back.setScaleType(ImageView.ScaleType.CENTER_INSIDE);
        this.addView(back);

        if(!TextUtils.isEmpty(rightText)){
            TextView tvRight = new TextView(getContext());
            LayoutParams rightParams = new LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT , ViewGroup.LayoutParams.MATCH_PARENT);
            rightParams.addRule(ALIGN_PARENT_RIGHT);
            tvRight.setLayoutParams(rightParams);
            tvRight.setTextColor(Color.parseColor("#ffffff"));
            tvRight.setTextSize(14);
            tvRight.setPadding(dp5 , 0 , dp5 , 0);
            tvRight.setGravity(Gravity.CENTER);
            tvRight.setText(rightText);
            this.addView(tvRight);
            tvRight.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    if(rightClickInter != null){
                        rightClickInter.clickRight();
                    }
                }
            });
        }

        back.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if(getContext() instanceof Activity){
                    ((Activity) getContext()).finish();
                }
            }
        });
    }

    private RightClickInter rightClickInter;
    public void setRightClickInter(RightClickInter rightClickInter){
        this.rightClickInter = rightClickInter;
    }

    public interface RightClickInter{
        void clickRight();
    }

    public void setTitile(String title){
        if(!TextUtils.isEmpty(title)){
            tvTitle.setText(title);
        }
    }

}
