package module.video.jnc.myffmpeg.activity;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import module.video.jnc.myffmpeg.MyGLSurfaceShader;
import module.video.jnc.myffmpeg.MyGlSurface;
import module.video.jnc.myffmpeg.R;

public class DecodeEncodeActivity extends AppCompatActivity {

    private TextView tv;
    private LinearLayout linearLayout;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_decode_encode);
        linearLayout= (LinearLayout) findViewById(R.id.container);
        tv = (TextView)findViewById(R.id.tv_warn);

        findViewById(R.id.bt_decode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                findViewById(R.id.bt_decode).setEnabled(false);
                MyGlSurface glSurface = new MyGlSurface(DecodeEncodeActivity.this);
                LinearLayout.LayoutParams parms = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT);
                linearLayout.removeAllViews();
                linearLayout.addView(glSurface , parms);
                tv.setText("解码结束");
            }
        });

        findViewById(R.id.bt_decode_show_shader).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                findViewById(R.id.bt_decode_show_shader).setEnabled(false);
                MyGLSurfaceShader glSurface = new MyGLSurfaceShader(DecodeEncodeActivity.this);
                LinearLayout.LayoutParams parms = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.MATCH_PARENT);
                linearLayout.removeAllViews();
                linearLayout.addView(glSurface , parms);
                tv.setText("解码结束");
            }
        });

        findViewById(R.id.bt_encode).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                findViewById(R.id.bt_encode).setEnabled(false);

            }
        });


    }
}
