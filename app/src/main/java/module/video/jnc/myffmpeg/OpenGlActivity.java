//package module.video.jnc.myffmpeg;
//
//import android.content.Intent;
//import android.support.v7.app.AppCompatActivity;
//import android.os.Bundle;
//import android.view.View;
//
//import module.video.jnc.myffmpeg.EGLCamera.EGLCameraActivity;
//import module.video.jnc.myffmpeg.opengl.OpenglActivity;
//
///**
// * 展示opengl功能模块
// *
// */
//public class OpenGlActivity extends AppCompatActivity {
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_open_gl);
//
//        findViewById(R.id.bt_opengl_texure).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                startActivity(new Intent(OpenGlActivity.this,  OpenGlTureActivity.class));
//            }
//        });
//
//        findViewById(R.id.bt_opengl).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                startActivity(new Intent(OpenGlActivity.this,  OpenglActivity.class));
//            }
//        });
//
//        findViewById(R.id.eglcamera).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                startActivity(new Intent(OpenGlActivity.this , EGLCameraActivity.class));
//            }
//        });
//        findViewById(R.id.load_jpg).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                startActivity(new Intent(OpenGlActivity.this , LoadJpgActivity.class));
//            }
//        });
//    }
//}
