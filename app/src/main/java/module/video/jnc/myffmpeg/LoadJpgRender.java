package module.video.jnc.myffmpeg;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.opengl.ShaderHelper;
import module.video.jnc.myffmpeg.opengl.TextResourceReader;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_LINEAR;
import static android.opengl.GLES20.GL_LINEAR_MIPMAP_LINEAR;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES20.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGenTextures;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform1i;
import static android.opengl.GLES20.glVertexAttribPointer;
import static android.opengl.GLES20.glViewport;

/**
 * Created by Administrator on 2018/4/11/011.
 */

public class LoadJpgRender implements GLSurfaceView.Renderer{
    private int mProgram;
    //    private int uColorLocation;
    private int aPositionLocation;
    private final FloatBuffer vertexData;
    private final FloatBuffer myTextureData;
    private static final int POSITION_COMPONENT_COUNT = 2;
    private static final int BYTES_PER_FLOAT = 4;
    private int aTextureCoordinatesLocation;

    private float[] myVertext = {-0.9f, -0.9f,
            -0.9f, 0.9f,
            0.9f, -0.9f,
            0.9f, -0.9f,
            0.9f, 0.9f,
            -0.9f, 0.9f};


    private float[] myTextureVertex = {
            0.1f, 0f,
            0.1f, 1f,
            0.9f, 0f,
            0.9f, 0f,
            0.9f, 1f,
            0.1f, 1f
    };
    Context context;

    public LoadJpgRender(Context context ){
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


    private int uTextureUnitLocation;

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        glClearColor(1f, 1f, 1f, 1f);
        String yuvFragment = TextResourceReader.readTextFileFromResource(context, R.raw.load_jpg_frgment_shader);
        String yuvVertex = TextResourceReader.readTextFileFromResource(context, R.raw.load_jpg_vertext_shader);
        mProgram = ShaderHelper.buildProgram(yuvVertex, yuvFragment);
        GLES20.glUseProgram(mProgram);
        aPositionLocation = glGetAttribLocation(mProgram, "a_Position");
        aTextureCoordinatesLocation = glGetAttribLocation(mProgram, "a_TextureCoordinates");
        uTextureUnitLocation = glGetUniformLocation(mProgram , "u_TextureUnit");
        vertexData.position(0);
        myTextureData.position(0);

        glVertexAttribPointer(aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, vertexData);
        glVertexAttribPointer(aTextureCoordinatesLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, myTextureData);

        glEnableVertexAttribArray(aPositionLocation);
        //使能纹理坐标
        glEnableVertexAttribArray(aTextureCoordinatesLocation);
        loadJpgTexure();
    }

    private int texture ;
    private int width = 1600 , height = 1050;

    private void loadJpgTexure(){
        final int[] textureObjectIds = new int[1];
        glGenTextures(1 ,textureObjectIds , 0);
        texture = textureObjectIds[0];
        glBindTexture(GL_TEXTURE_2D , texture);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        glActiveTexture(GLES20.GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D , texture);
        getAssetFile();
        mallocBytes();
//        GLES20.glTexImage2D(GL_TEXTURE_2D , 0 , GLES20.GL_LUMINANCE , width, height ,0,  GLES20.GL_ALPHA , GLES20.GL_UNSIGNED_BYTE , byteBuffer);
    }
    private  byte[] myFileBytes;

    private ByteBuffer byteBuffer;

    private void mallocBytes(){
        byteBuffer =  ByteBuffer.allocateDirect(myFileBytes.length ) ;
        System.arraycopy(myFileBytes , 0 , myFileBytes , 0 ,myFileBytes.length );
        byteBuffer.put(myFileBytes);
        byteBuffer.position(0);
    }
    private void updateImage(){

        glActiveTexture(GLES20.GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D , texture);
        GLES20.glTexImage2D(GL_TEXTURE_2D , 0 , GLES20.GL_LUMINANCE , width, height ,0,  GLES20.GL_ALPHA , GLES20.GL_UNSIGNED_BYTE , byteBuffer);
        glUniform1i(uTextureUnitLocation ,0);



    }
    private void getAssetFile() {
        AssetManager am = context.getAssets();
        ByteArrayOutputStream bos = null;
        InputStream is = null;
        byte[] temp = new byte[1024];
        try {
            is = am.open("test.jpg");
            bos = new ByteArrayOutputStream(is.available());
            int len = -1;
            while ((len = is.read(temp, 0, temp.length)) != -1) {
                bos.write(temp, 0, len);
            }
            myFileBytes = bos.toByteArray();
            Log.e("xhc" , " file byte size "+myFileBytes.length);
        } catch (Exception e) {

        } finally {
            try {
                is.close();
            } catch (Exception e) {
            }
            try {
                bos.close();
            } catch (Exception e) {

            }

        }

    }
    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {
        glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);

        updateImage();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    }
}
