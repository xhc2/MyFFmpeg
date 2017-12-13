package module.video.jnc.myffmpeg.opengl;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glVertexAttribPointer;

/**
 * Created by xhc on 2017/12/13.
 */

public class VertextArray {

    /**
     * 用来在本地存储本地数据的
     */
    private final FloatBuffer floatBuffer;

    public VertextArray(float[] vertextData){
        floatBuffer = ByteBuffer.allocateDirect(vertextData.length * Constant.BYTES_PER_FLOAT)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertextData);
    }

    public void setVertexAttribPointer(int dataOffset , int attributeLocation , int componetCount , int stride){
        floatBuffer.position(dataOffset);
        glVertexAttribPointer(attributeLocation , componetCount , GL_FLOAT , false , stride , floatBuffer);
        glEnableVertexAttribArray(attributeLocation);
        floatBuffer.position(0);
    }

}
