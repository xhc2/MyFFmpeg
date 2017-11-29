package module.video.jnc.myffmpeg;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

/**
 * 添加水印
 */
public class AddFilterActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_filter);
        //添加图片水印
        findViewById(R.id.bt_add_filter).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                FFmpegUtils.addfilter(Constant.rootFile.getAbsolutePath()+"/test.mp4" , Constant.rootFile.getAbsolutePath()+"/filter.yuv");
            }
        });
    }

}
