package module.video.jnc.myffmpeg.activity;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

import module.video.jnc.myffmpeg.FFmpegUtils;
import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.tool.FileUtils;

public class FlvParseActivity extends BaseActivity {

    private TextView tvData;
    private EditText etText;
    private ProgressDialog dialog;
    private String path ="sdcard/FFmpeg/test.aac";

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
        FileUtils.makeFileParse();
        tvData = findViewById(R.id.tv_all_data);
        etText = findViewById(R.id.et_path);
        etText.setText(path);

        dialog = new ProgressDialog(this);
        dialog.setMessage("解析中...");

        findViewById(R.id.bt_flv_parse).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String str = etText.getText().toString();
                if(str.endsWith("flv")){

                    startThread(etText.getText().toString());
                }
                else{
                    Toast.makeText(FlvParseActivity.this, "请输入正确文件格式", Toast.LENGTH_SHORT).show();
                }

            }
        });

        findViewById(R.id.bt_parse_h264).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //分析h264
                String str = etText.getText().toString();
                if(str.endsWith("h264") || str.endsWith("264")){

                    startThread(etText.getText().toString());
                }
                else{
                    Toast.makeText(FlvParseActivity.this, "请输入正确文件格式", Toast.LENGTH_SHORT).show();
                }
            }
        });

        findViewById(R.id.bt_parse_aac).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String str = etText.getText().toString();
                if(str.endsWith("aac")){

                    startThread(etText.getText().toString());
                }
                else{
                    Toast.makeText(FlvParseActivity.this, "请输入正确文件格式", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private ParseThread ptThread;


    private void startThread(String path){
        if(!new File(path).exists()){
            showToast("文件不存在！");
            return ;
        }
        dialog.show();
        stopParseThread();
        if(ptThread == null){
            ptThread = new ParseThread(path);
            ptThread.start();
        }

    }
    private void stopParseThread(){
        if(ptThread != null){
            try {
                ptThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        ptThread = null;
    }
    class ParseThread extends Thread{
        String path ;
        ParseThread(String p){
            this.path = p;
        }

        @Override
        public void run() {
            super.run();
            String result = null;
            if(path.endsWith("flv")){
                result = FFmpegUtils.flvParse(path);
            }
            else if(path.endsWith("h264") || path.endsWith("264")){
                result = FFmpegUtils.h264Parse(path);
            }
            else if(path.endsWith("aac")){
                result = FFmpegUtils.aacParse(path);
            }
            Message msg = new Message();
            msg.obj = result;
            handler.sendMessage(msg);
        }
    }

}
