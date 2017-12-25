package module.video.jnc.myffmpeg.EGLCamera;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.opengl.ShaderHelper;
import module.video.jnc.myffmpeg.opengl.TextResourceReader;

import static android.opengl.GLES20.glEnable;
import static android.opengl.GLES20.glUseProgram;

/**
 * Created by xhc on 2017/12/25.
 */

public class DirectDrawer {

    private FloatBuffer vertextBuffer , textureVerticesBuffer;
    private ShortBuffer drawListBuffer ;
    private int program;
    private int mPositionHandle;
    private int mTextureCoordHandle;

    private Context context ;
    private short drawOrder[] = {0 , 1 ,2  ,0 ,2 ,3};
    private static final int COORDS_PER_VERTEX = 2;

    private final int vertexStride = COORDS_PER_VERTEX * 4 ;
    static float squareCoords[] = {
            -1.0f , 1.0f,
            -1.0f , -1.0f,
            1.0f ,  -1.0f ,
            1.0f ,  1.0f
    };
    static float textureVertices[] = {
            0.0f , 1.0f ,
            1.0f , 1.0f ,
            1.0f , 0.0f ,
            0.0f , 0.0f
    };
    private int texture ;

    public DirectDrawer(int texture , Context context){
        this.context = context;
        this.texture = texture;
        ByteBuffer bb = ByteBuffer.allocateDirect(squareCoords.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertextBuffer = bb.asFloatBuffer();
        vertextBuffer.put(squareCoords);
        vertextBuffer.position(0);

        // initialize byte buffer for the draw list
        ByteBuffer dlb = ByteBuffer.allocateDirect(drawOrder.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);


        ByteBuffer bb2 = ByteBuffer.allocateDirect(textureVertices.length * 4);
        bb2.order(ByteOrder.nativeOrder());
        textureVerticesBuffer = bb2.asFloatBuffer();
        textureVerticesBuffer.put(textureVertices);
        textureVerticesBuffer.position(0);

        String vertexShaderCode = TextResourceReader.readTextFileFromResource(context , R.raw.camera_vertex_shader);
        String fragmentShaderCode = TextResourceReader.readTextFileFromResource(context , R.raw.camera_fragment_shader);


        //编译两个程序
        int vertexShader = ShaderHelper.compileVertextShader(vertexShaderCode);
        int fragmentShader = ShaderHelper.compileFragmentShader(fragmentShaderCode);
        Log.e("xhc" , " vertexShader "+vertexShader+" fragmentShader "+fragmentShader);
        if(vertexShader != 0 && fragmentShader != 0){
            //链接到程序
            program = ShaderHelper.linkProgram(vertexShader  ,fragmentShader );
        }

        //验证程序是否可用
        ShaderHelper.validatePrograme(program);
        //使用程序
//        glUseProgram(program);
//
//        mPositionHandle = GLES20.glGetAttribLocation(program, "vPosition");
//
//        // Enable a handle to the triangle vertices
//        GLES20.glEnableVertexAttribArray(mPositionHandle);
//
//        // Prepare the <insert shape here> coordinate data
//        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, vertexStride, vertextBuffer);

    }

    public void draw(float[] mtx){
        glUseProgram(program);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture);
        // get handle to vertex shader's vPosition member
        mPositionHandle = GLES20.glGetAttribLocation(program, "vPosition");

        // Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);

        // Prepare the <insert shape here> coordinate data
        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, vertexStride, vertextBuffer);

        mTextureCoordHandle = GLES20.glGetAttribLocation(program, "inputTextureCoordinate");
        GLES20.glEnableVertexAttribArray(mTextureCoordHandle);

        GLES20.glVertexAttribPointer(mTextureCoordHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, vertexStride, textureVerticesBuffer);

        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer);

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle);
        GLES20.glDisableVertexAttribArray(mTextureCoordHandle);
    }
    private float[] transformTextureCoordinates( float[] coords, float[] matrix)
    {
        float[] result = new float[ coords.length ];
        float[] vt = new float[4];

        for ( int i = 0 ; i < coords.length ; i += 2 ) {
            float[] v = { coords[i], coords[i+1], 0 , 1  };
            Matrix.multiplyMV(vt, 0, matrix, 0, v, 0);
            result[i] = vt[0];
            result[i+1] = vt[1];
        }
        return result;
    }
}




