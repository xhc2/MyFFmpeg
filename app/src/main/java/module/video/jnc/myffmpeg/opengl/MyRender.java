package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.R;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_LINES;
import static android.opengl.GLES20.GL_LINE_LOOP;
import static android.opengl.GLES20.GL_POINTS;
import static android.opengl.GLES20.GL_TRIANGLES;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform4f;
import static android.opengl.GLES20.glUseProgram;
import static android.opengl.GLES20.glVertexAttribPointer;
import static android.opengl.GLES20.glViewport;
/**
 * Created by xhc on 2017/12/7.
 * opengl 是以屏幕中心为坐标原点的。
 */

public class MyRender implements GLSurfaceView.Renderer{

    private Context context;
    private int program ;
    private static final String U_COLOR = "u_Color";
    private int uColorLocation ;

    private static final String A_POSITION = "a_Position";
    private int aPositionLocation ;

    private static final int BYTES_PER_FLOAT = 4;
    private final FloatBuffer vertexData;
    float[] tableVerticesWithTriangles = {
            -0.5f,-0.5f,
            0.5f,0.5f,
            -0.5f,0.5f ,

            -0.5f,-0.5f,
            0.5f,-0.5f ,
            0.5f,0.5f ,

            -0.5f , 0f ,
            0.5f , 0f ,

            0f , -0.25f ,

            0f , 0.25f };

    private static final int POSITION_COMPONENT_COUNT = 2;
    public MyRender(Context context){
        this.context = context;
        vertexData = ByteBuffer.allocateDirect(tableVerticesWithTriangles.length * BYTES_PER_FLOAT )
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        vertexData.put(tableVerticesWithTriangles);
//        float[] tableVerticec = {0f , 0f , 0f , 14f , 9f , 14f , 9f , 0f};
    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e("xhc" , " render surface create ");
        glClearColor(0.0f , 0.0f ,0.0f,0.0f);
        //加载顶点着色器
        String vertexShaderSource = TextResourceReader.readTextFileFromResource(context , R.raw.simple_vertex_shader);
        //加载片段着色器
        String fragmentShaderSource = TextResourceReader.readTextFileFromResource(context , R.raw.simple_fragment_shader);

        //编译两个程序
        int vertexShader = ShaderHelper.compileVertextShader(vertexShaderSource);
        int fragmentShader = ShaderHelper.compileFragmentShader(fragmentShaderSource);
        Log.e("xhc" , " vertexShader "+vertexShader+" fragmentShader "+fragmentShader);

        if(vertexShader != 0 && fragmentShader != 0){
            //链接到程序
            program = ShaderHelper.linkProgram(vertexShader  ,fragmentShader );
        }


        //验证程序是否可用
        ShaderHelper.validatePrograme(program);
        //使用程序
        glUseProgram(program);
        //找到颜色属性的位置，绘制颜色的时候要使用
        uColorLocation = glGetUniformLocation(program , U_COLOR);
        //找到位置属性的位置
        aPositionLocation = glGetAttribLocation(program , A_POSITION);
        Log.e("xhc" , " aPositionLocation "+aPositionLocation);
        vertexData.position(0);
        //aPositionLocation 属性的位置，
        // POSITION_COMPONENT_COUNT数据的计数两个，x，y。
//        GL_FLOAT数据的类型
        glVertexAttribPointer(aPositionLocation , POSITION_COMPONENT_COUNT , GL_FLOAT , false , 0 , vertexData);
        glEnableVertexAttribArray(aPositionLocation);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        glViewport(0 , 0 , i , i1);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform4f(uColorLocation ,1.0f ,1.0f ,1.0f,1.0f);
        glDrawArrays(GL_TRIANGLES  , 0 , 6);

        glUniform4f(uColorLocation ,1.0f ,0f ,0f,1.0f);
        glDrawArrays(GL_LINES  , 6 , 2);

        glUniform4f(uColorLocation ,0.0f ,0.0f ,1.0f,1.0f);
        glDrawArrays(GL_POINTS  , 8 , 1);

        glUniform4f(uColorLocation ,1.0f ,0.0f ,0.0f,1.0f);
        glDrawArrays(GL_POINTS , 9 , 1);
    }
}
