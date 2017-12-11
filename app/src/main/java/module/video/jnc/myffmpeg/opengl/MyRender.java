package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.R;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glUseProgram;
import static android.opengl.GLES20.glViewport;

/**
 * Created by xhc on 2017/12/7.
 */

public class MyRender implements GLSurfaceView.Renderer{

    private Context context;
    private int program ;
    public MyRender(Context context){
        this.context = context;
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e("xhc" , " render surface create ");
        glClearColor(255.0f , 255.0f ,255.0f,255.0f);
        String vertexShaderSource = TextResourceReader.readTextFileFromResource(context , R.raw.simple_vertex_shader);
        String fragmentShaderSource = TextResourceReader.readTextFileFromResource(context , R.raw.simple_fragment_shader);

        int vertexShader = ShaderHelper.compileVertextShader(vertexShaderSource);
        int fragmentShader = ShaderHelper.compileFragmentShader(fragmentShaderSource);
        Log.e("xhc" , " vertexShader "+vertexShader+" fragmentShader "+fragmentShader);
        if(vertexShader != 0 && fragmentShader != 0){
            program = ShaderHelper.linkProgram(vertexShader  ,fragmentShader );
        }
        //验证程序是否可用
        ShaderHelper.validatePrograme(program);
        glUseProgram(program);

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        glViewport(0 , 0 , i , i1);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
