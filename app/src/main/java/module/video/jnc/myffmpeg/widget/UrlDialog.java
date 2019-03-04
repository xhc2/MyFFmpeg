package module.video.jnc.myffmpeg.widget;

import android.app.Dialog;
import android.content.Context;
import android.support.annotation.NonNull;
import android.view.View;
import android.widget.EditText;

import module.video.jnc.myffmpeg.R;

public class UrlDialog extends Dialog {

    private ClickConfirm lis ;

    public interface ClickConfirm{
        void clickConfirm(String url);
    }

    public UrlDialog(@NonNull Context context  ,ClickConfirm lis) {
        super(context ,R.style.dialog);
        setContentView(R.layout.input_url_layout);
        this.lis = lis;
        findViewById(R.id.bt_confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(UrlDialog.this.lis != null){
                    UrlDialog.this.lis.clickConfirm(((EditText)findViewById(R.id.et_url)).getText().toString());
                }
            }
        });
    }





}
