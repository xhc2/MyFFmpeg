package module.video.jnc.myffmpeg;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.opengl.ShaderHelper;
import module.video.jnc.myffmpeg.opengl.TextResourceReader;
import module.video.jnc.myffmpeg.opengl.TextureHelper;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.GL_TRIANGLES;
import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform1i;
import static android.opengl.GLES20.glUniform4f;
import static android.opengl.GLES20.glVertexAttribPointer;
import static android.opengl.GLES20.glViewport;

/**
 * Created by Administrator on 2018/4/7/007.
 */

public class PlayYuvRender implements GLSurfaceView.Renderer {

    private Context context;
    private int mProgram;
//    private int uColorLocation;
    private int aPositionLocation;
    private final FloatBuffer vertexData;
    private final FloatBuffer myTextureData;
    private static final int POSITION_COMPONENT_COUNT = 2;
    private static final int BYTES_PER_FLOAT = 4;
    private int uTextureUnitLocation;
    private int myTexture;

    private float[] myVertext = {-0.9f, -0.9f,
            -0.9f, 0.9f,
            0.9f, -0.9f,
            0.9f, -0.9f,
            0.9f, 0.9f,
            -0.9f, 0.9f};


    private float[] myTextureVertex = {
            0f , 0f ,
            0f , 0.9f ,
            0.9f, 0f ,
            0.9f, 0f ,
            0.9f , 0.9f ,
            0f ,0.9f
    };
    public PlayYuvRender(Context context) {
        this.context = context;
        vertexData = ByteBuffer.allocateDirect(myVertext.length * BYTES_PER_FLOAT)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        vertexData.put(myVertext);

        myTextureData = ByteBuffer.allocateDirect(myTextureVertex.length * BYTES_PER_FLOAT)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer();
        myTextureData.put(myTextureVertex);
    }

    private int aTextureCoordinatesLocation;
    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        glClearColor(1f, 1f, 1f, 1f);
        String yuvFragment = TextResourceReader.readTextFileFromResource(context, R.raw.yuv_frg_shader);
        String yuvVertex = TextResourceReader.readTextFileFromResource(context, R.raw.yuv_vertex_shader);
        mProgram = ShaderHelper.buildProgram(yuvVertex, yuvFragment);
        GLES20.glUseProgram(mProgram);

        aPositionLocation = glGetAttribLocation(mProgram, "a_Position");
        uTextureUnitLocation = glGetUniformLocation(mProgram, "u_TextureUnit");
        //纹理坐标
        aTextureCoordinatesLocation = glGetAttribLocation(mProgram , "a_TextureCoordinates");
        vertexData.position(0);
        myTextureData.position(0);
        glVertexAttribPointer(aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, vertexData);
        glVertexAttribPointer(aTextureCoordinatesLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, myTextureData);
        glEnableVertexAttribArray(aPositionLocation);
        //使能纹理坐标
        glEnableVertexAttribArray(aTextureCoordinatesLocation);
        myTexture = TextureHelper.loadTexture(context, R.mipmap.air_hockey_surface);
//        Log.e("xhc" , " uTextureUnitLocation "+uTextureUnitLocation);


    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        glViewport(0, 0, width, height);
    }


    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D , myTexture);
        //uTextureUnitLocation，纹理数据的位置，0是对应的GL_TEXTURE0纹理单元
        glUniform1i(uTextureUnitLocation , 0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    }
}
