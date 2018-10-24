package module.video.jnc.myffmpeg.activity;

import android.app.ProgressDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

public class BaseActivity extends AppCompatActivity {

    private ProgressDialog dialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    protected void showDialog(String msg){
        if(dialog == null){
            dialog = new ProgressDialog(this);
        }
        dialog.setMessage(msg);
        dialog.show();
    }

    protected void dismissDialog(){
        if(dialog != null && dialog.isShowing()){
            dialog.dismiss();
        }
    }

    protected void showToast(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        dismissDialog();
    }
}
