package module.video.jnc.myffmpeg.widget;

import android.app.Dialog;
import android.content.Context;
import android.support.annotation.NonNull;
import android.widget.ProgressBar;
import android.widget.TextView;

import module.video.jnc.myffmpeg.R;

public class LoadProgressDialog extends Dialog {

    private ProgressBar pb;
    private TextView tv;
    private String msg;
    public LoadProgressDialog(@NonNull Context context , String msg) {
        super(context , R.style.dialog);
        setContentView(R.layout.dig_load_progress_layout);
//        setCancelable(false);
        setCanceledOnTouchOutside(false);
        this.msg = msg;
        init();

    }
    private void init(){
        pb = findViewById(R.id.progress_bar);
        tv = findViewById(R.id.tv_loading);
        tv.setText(msg);
    }
    public void setProgress(int progress){
        pb.setProgress(progress);
    }



}
