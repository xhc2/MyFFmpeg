package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
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
import static android.opengl.GLES20.GL_POINTS;
import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniformMatrix4fv;
import static android.opengl.GLES20.glUseProgram;
import static android.opengl.GLES20.glVertexAttribPointer;
import static android.opengl.GLES20.glViewport;

/**
 * Created by xhc on 2017/12/13.
 */

public class NewMyRender implements GLSurfaceView.Renderer {

    private Context context;
    private final float[] projectionMatrix = new float[16];
    private final float[] modelMatrix = new float[16];
    private Table table ;
    private Mallet mallet;
    private TextureShaderProgram textureShaderProgram ;
    private ColorShaderProgram colorShaderProgram;
    private int textture;

    private final float[] viewMatrix = new float[16];
    private final float[] viewProjectionMatrix = new float[16];
    private final float[] modelViewProjectionMatrix = new float[16];

    private Puck puck;

    public NewMyRender(Context context) {
        this.context = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        Log.e("xhc", " render surface create ");
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        table = new Table();
        mallet = new Mallet(0.08f , 0.15f , 32);
        puck = new Puck(0.06f , 0.02f , 32);
        textureShaderProgram = new TextureShaderProgram(context);
        colorShaderProgram = new ColorShaderProgram(context);
        textture = TextureHelper.loadTexture(context , R.mipmap.air_hockey_surface);

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        glViewport(0, 0, width, height);
        //这会用45度的视野创建一个透视投影，这个视椎体从z值-1的位置开始，到z值为-10的位置结束
//        final float aspectRation = width > height ? (float) width / (float) height : (float)height / (float)width;
        /**
         * float[] m, int mOffset, float left, float right, float bottom, float top, float near, float far
         * m:目标数组
         * mOffset：结果矩阵的其实偏移值
         * left: x轴最小的范围
         * right：x轴最大的范围
         * bottom：y轴最小的范围
         * top：y轴最大的范围
         * near：z轴最小的范围
         * far：z轴最大的范围
         * 这个函数就是生成一个正交矩阵，就是将以前在屏幕上的坐标范围（-1，1）改变下e.g（-1.78,1.78）。
         * 如果按上述方式改变下就是相对就更“聚拢”了。
         */

//        Log.e("xhc" , "aspectRation "+aspectRation);
//        if(width > height){
//            Matrix.orthoM(projectionMatrix , 0 ,  -aspectRation , aspectRation ,-1f, 1f , -1f , 1f);
////            Matrix.orthoM(projectionMatrix , 0 ,  -3f , 3f ,-1f, 1f , -1f , 1f);
//        }
//        else{
//            Matrix.orthoM(projectionMatrix , 0 , -1f, 1f , -aspectRation , aspectRation , -1f , 1f);
//        }

        MatrixHelper.perspectiveM(projectionMatrix, 45, (float) width / (float) height, 1f, 10f);
        Matrix.setLookAtM(viewMatrix , 0 , 0f , 1.2f , 2.2f , 0f , 0f , 0f , 0f , 1f , 0f );

//        Matrix.setIdentityM(modelMatrix, 0);
//        //利用模型矩阵移动物体，沿z轴负方向平移-2
//        Matrix.translateM(modelMatrix, 0, 0f, 0f, -2.5f);
//        //旋转
//        Matrix.rotateM(modelMatrix , 0 , -60f , 1f , 0f , 0f);
//        //投影矩阵乘以模型矩阵。
//        final float[] temp = new float[16];
//        Matrix.multiplyMM(temp, 0, projectionMatrix, 0, modelMatrix, 0);
//        System.arraycopy(temp, 0, projectionMatrix, 0, temp.length);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);

        Matrix.multiplyMM(viewProjectionMatrix , 0 , projectionMatrix , 0 , viewMatrix , 0);
        positionTableInScene();
        textureShaderProgram.useProgram();
        textureShaderProgram.setUniforms(modelViewProjectionMatrix , textture);
        table.bindData(textureShaderProgram);
        table.draw();

        positionObjectInScene(0f , mallet.height / 2f , -0.4f);
        colorShaderProgram.useProgram();
        colorShaderProgram.setUniforms(modelViewProjectionMatrix , 1f , 0f , 0f);
        mallet.bindData(colorShaderProgram);
        mallet.draw();

        positionObjectInScene(0f , mallet.height / 2f , 0.4f);
        colorShaderProgram.setUniforms(modelViewProjectionMatrix , 0f , 0f , 1f);
        mallet.draw();

        positionObjectInScene(0f , puck.height / 2f , 0f);
        colorShaderProgram.setUniforms(modelViewProjectionMatrix , 0.8f , 0.8f , 1f);
        puck.bindData(colorShaderProgram);
        puck.draw();
//        textureShaderProgram.useProgram();
//        textureShaderProgram.setUniforms(projectionMatrix , textture);
//        table.bindData(textureShaderProgram);
//        table.draw();
//
//        colorShaderProgram.useProgram();
//        colorShaderProgram.setUniforms(projectionMatrix);
//        mallet.bindData(colorShaderProgram);
//        mallet.draw();
    }

    private void positionTableInScene(){
        Matrix.setIdentityM(modelMatrix , 0);
        Matrix.rotateM(modelMatrix , 0 , -90f , 1f , 0f , 0f);
        Matrix.multiplyMM(modelViewProjectionMatrix , 0  , viewProjectionMatrix , 0 , modelMatrix , 0);
    }

    private void positionObjectInScene(float x , float y , float z){
        Matrix.setIdentityM(modelMatrix , 0);
        Matrix.translateM(modelMatrix , 0 , x , y , z);
        Matrix.multiplyMM(modelViewProjectionMatrix , 0 , viewProjectionMatrix , 0 , modelMatrix , 0);
    }

}