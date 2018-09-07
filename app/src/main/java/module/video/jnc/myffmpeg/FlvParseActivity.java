package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class FlvParseActivity extends Activity {

    private TextView tvData;
    private EditText etText;
    private ProgressDialog dialog;
    
    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message msg) {
            dialog.dismiss();
            String str = (String)msg.obj;
            tvData.setText(str);
            return false;
        }
    });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_flv_parse);
        tvData = findViewById(R.id.tv_all_data);
        etText = findViewById(R.id.et_path);
        dialog = new ProgressDialog(this);
        dialog.setMessage("解析中...");
        findViewById(R.id.bt_parse).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.show();
                startThread(etText.getText().toString());
            }
        });
    }

    private ParseThread ptThread;


    private void startThread(String path){
        if(ptThread == null){
            ptThread = new ParseThread(path);
            ptThread.start();
        }

    }

    class ParseThread extends Thread{
        String path ;
        ParseThread(String p){
            this.path = p;
        }

        @Override
        public void run() {
            super.run();
            String result = FFmpegUtils.flvParse(path);
            Message msg = new Message();
            msg.obj = result;
            handler.sendMessage(msg);
        }
    }

}
