package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class FlvParseActivity extends Activity {

    private TextView tvData;
    private EditText etText;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_flv_parse);
        tvData = findViewById(R.id.tv_all_data);
        etText = findViewById(R.id.et_path);

        findViewById(R.id.bt_parse).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String result = FFmpegUtils.flvParse(etText.getText().toString());
                tvData.setText(result);
            }
        });


    }
}
