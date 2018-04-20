//package module.video.jnc.myffmpeg;
//
//import android.support.v7.app.AppCompatActivity;
//import android.os.Bundle;
//import android.view.View;
//
///**
// * 将yuv格式编码成MP4
// */
//public class EncodeYuvActivity extends AppCompatActivity {
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_encode_yuv);
//        findViewById(R.id.bt_start).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                FFmpegUtils.encodeYuv(Constant.rootFile.getAbsolutePath()+"/test_480_272.yuv" ,
//                        Constant.rootFile.getAbsolutePath()+"/encodeyuv.MP4");
//            }
//        });
//
//
//    }
//}
