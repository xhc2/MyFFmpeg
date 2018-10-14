package module.video.jnc.myffmpeg.widget;

import android.app.Activity;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import module.video.jnc.myffmpeg.DensityUtils;
import module.video.jnc.myffmpeg.R;

public class TitleBar extends RelativeLayout{

    private TextView tvTitle;
    private String title ;
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
        tvTitle.setText(title);
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

        back.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if(getContext() instanceof Activity){
                    ((Activity) getContext()).finish();
                }
            }
        });
    }

}
