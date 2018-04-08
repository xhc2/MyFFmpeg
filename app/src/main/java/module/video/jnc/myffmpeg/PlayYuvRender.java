package module.video.jnc.myffmpeg;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.opengl.ShaderHelper;
import module.video.jnc.myffmpeg.opengl.TextResourceReader;
import module.video.jnc.myffmpeg.opengl.TextureHelper;

import static android.opengl.GLES10.GL_MAX_TEXTURE_UNITS;
import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.GL_LINEAR;
import static android.opengl.GLES20.GL_LINEAR_MIPMAP_LINEAR;
import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE1;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES20.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES20.GL_TRIANGLES;
import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glDisableVertexAttribArray;
import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glGenTextures;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetIntegerv;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glTexParameteri;
import static android.opengl.GLES20.glUniform1i;
import static android.opengl.GLES20.glUniform4f;
import static android.opengl.GLES20.glVertexAttribPointer;
import static android.opengl.GLES20.glViewport;
import static android.opengl.GLUtils.texImage2D;

/**
 * Created by Administrator on 2018/4/7/007.
 * https://blog.csdn.net/leixiaohua1020/article/details/40379845
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
    private int myTextureY , myTextureU , myTextureV;

    private ByteBuffer byteBufferY,byteBufferU,byteBufferV;
    private int yLength , uLength , vLength;
    private  byte[] myFileBytes;
    private byte[] byteY , byteU , byteV;
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

//    private float[] myVertext= {
//            -1.0f, -1.0f,
//            1.0f, -1.0f,
//            -1.0f,  1.0f,
//            1.0f,  1.0f,
//    };
//
//    private float[] myTextureVertex = {
//            0.0f,  1.0f,
//            1.0f,  1.0f,
//            0.0f,  0.0f,
//            1.0f,  0.0f,
//    };

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
    private int yLocation , uLocation , vLocation;
    private int width = 256 , height = 256;

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        glClearColor(1f, 1f, 1f, 1f);
        getAssetFile();
        yLength = myFileBytes.length * 2 / 3;
        uLength = myFileBytes.length * 1 / 6;
        vLength = myFileBytes.length * 1 / 6;
        Log.e("xhc" , " yLength "+yLength+" uLength "+uLength+" vLength "+vLength);
        mallocYuvBuffer();
        loadYuvTexture();
        String yuvFragment = TextResourceReader.readTextFileFromResource(context, R.raw.yuv_frg_shader);
        String yuvVertex = TextResourceReader.readTextFileFromResource(context, R.raw.yuv_vertex_shader);
        mProgram = ShaderHelper.buildProgram(yuvVertex, yuvFragment);
        GLES20.glUseProgram(mProgram);

        aPositionLocation = glGetAttribLocation(mProgram, "a_Position");
        //纹理坐标
        aTextureCoordinatesLocation = glGetAttribLocation(mProgram, "a_TextureCoordinates");
        vertexData.position(0);
        myTextureData.position(0);

        glVertexAttribPointer(aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, vertexData);
        glVertexAttribPointer(aTextureCoordinatesLocation, POSITION_COMPONENT_COUNT, GL_FLOAT, false, 0, myTextureData);

        glEnableVertexAttribArray(aPositionLocation);
        //使能纹理坐标
        glEnableVertexAttribArray(aTextureCoordinatesLocation);

        yLocation = glGetUniformLocation(mProgram, "textureY");
        uLocation = glGetUniformLocation(mProgram, "textureU");
        vLocation = glGetUniformLocation(mProgram, "textureV");

        Log.e("xhc" , "  yLocation "+yLocation+" uLocation "+uLocation+" vLocation "+vLocation);

    }


    private void mallocYuvBuffer(){
        byteY = new byte[yLength];
        byteU = new byte[uLength];
        byteV = new byte[vLength];

        byteBufferY =  ByteBuffer.allocateDirect(yLength ) ;
        System.arraycopy(myFileBytes , 0 , byteY , 0 ,yLength);
        byteBufferY.put(byteY);
        byteBufferY.position(0);

        byteBufferU =  ByteBuffer.allocateDirect(uLength)  ;
        System.arraycopy(myFileBytes , width * height , byteU , 0 ,uLength);
        byteBufferU.put(byteU);
        byteBufferU.position(0);


        byteBufferV =  ByteBuffer.allocateDirect(vLength) ;
        System.arraycopy(myFileBytes , width * height + (width * height / 4) , byteV , 0 ,vLength);
        byteBufferV.put(byteV);
        byteBufferV.position(0);

//        File fileY = new File(Constant.rootFile.getAbsoluteFile()+"/oepnGltemp.y");
//        File fileU = new File(Constant.rootFile.getAbsoluteFile()+"/openGltemp.u");
//        File fileV = new File(Constant.rootFile.getAbsoluteFile()+"/openGltemp.v");
//        try{
//            fileY.createNewFile();
//            fileU.createNewFile();
//            fileV.createNewFile();
//            FileOutputStream fos = new FileOutputStream(fileY);
//            fos.write(byteY);
//            fos.write(byteU);
//            fos.write(byteV);
//            FileOutputStream fosu = new FileOutputStream(fileU);
//            fosu.write(byteU);
//            FileOutputStream fosv = new FileOutputStream(fileV);
//            fosv.write(byteV);
//        }
//        catch(Exception e){
//
//        }
    }

    private int loadYuvTexture(){
        final int[] textureObjectIds = new int[1];

        glGenTextures(1 ,textureObjectIds , 0);
        myTextureY = textureObjectIds[0];
        glBindTexture(GL_TEXTURE_2D , myTextureY);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        glGenTextures(1 ,textureObjectIds , 0);
        myTextureU = textureObjectIds[0];
        glBindTexture(GL_TEXTURE_2D , myTextureU);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        glGenTextures(1 ,textureObjectIds , 0);
        myTextureV = textureObjectIds[0];
        glBindTexture(GL_TEXTURE_2D , myTextureV);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
        GLES20.glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        return 1;
    }

    private void disableVertextArray(){
        glDisableVertexAttribArray(aPositionLocation);
        //使能纹理坐标
        glDisableVertexAttribArray(aTextureCoordinatesLocation);
    }


    private void updateImage(){
        byteBufferY.position(0);
        byteBufferU.position(0);
        byteBufferV.position(0);

        glActiveTexture(GLES20.GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D , myTextureY);
        GLES20.glTexImage2D(GL_TEXTURE_2D , 0 , GLES20.GL_LUMINANCE , width, height ,0,  GLES20.GL_ALPHA , GLES20.GL_UNSIGNED_BYTE , byteBufferY);
        glUniform1i(yLocation ,0);

        glActiveTexture(GLES20.GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D , myTextureU);
        GLES20.glTexImage2D(GL_TEXTURE_2D , 0 , GLES20.GL_LUMINANCE , width /2 , height /2  ,0,   GLES20.GL_ALPHA , GLES20.GL_UNSIGNED_BYTE , byteBufferU);
        glUniform1i(uLocation ,1);

        glActiveTexture(GLES20.GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D , myTextureV);
        GLES20.glTexImage2D(GL_TEXTURE_2D , 0 , GLES20.GL_LUMINANCE, width /2 , height /2  ,0,  GLES20.GL_ALPHA , GLES20.GL_UNSIGNED_BYTE  , byteBufferV);
        glUniform1i(vLocation ,2);

    }

    private int getMaxTextrueSize() {
        int[] tmpi = new int[2];
        glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, tmpi, 0);
        Log.e("xhc", " 最大的纹理单元数 " + tmpi[0]);
        return tmpi[0];
    }

    private void getAssetFile() {
        AssetManager am = context.getAssets();
        ByteArrayOutputStream bos = null;
        InputStream is = null;
        byte[] temp = new byte[1024];
        try {
            is = am.open("lena_256x256_yuv420p.yuv");
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
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        glViewport(0, 0, width, height);
    }


    @Override
    public void onDrawFrame(GL10 gl10) {
        glClear(GL_COLOR_BUFFER_BIT);

        //glBindTexture(GL_TEXTURE_2D, myTextureY);

        //uTextureUnitLocation，纹理数据的位置，0是对应的GL_TEXTURE0纹理单元
        //glUniform1i(uTextureUnitLocation, 0);

        updateImage();
//        glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    }
}
