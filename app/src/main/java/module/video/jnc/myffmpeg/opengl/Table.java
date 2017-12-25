package module.video.jnc.myffmpeg.opengl;


import android.opengl.GLES20.*;

import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.glDrawArrays;

/**
 * Created by xhc on 2017/12/13.
 */

public class Table {

    private static final int POSITION_COMPONENT_COUNT = 2;
    private static final int TEXTURE_COORDINATES_COMPONNET_COUNT = 2;

    //跨度，告诉opengly一组需要多少个点
    private static final int STRIDE = (POSITION_COMPONENT_COUNT + TEXTURE_COORDINATES_COMPONNET_COUNT) * Constant.BYTES_PER_FLOAT;

    private final VertextArray vertextArray;

    private static final float[] VERTEX_DATA = {
            //x , y , s , t, (st是纹理的坐标)
            0f, 0f, 0.5f, 0.5f,
            -0.5f, -0.8f, 0f, 0.9f,
            0.5f, -0.8f, 1f, 0.9f,

            0.5f, 0.8f, 1f, 0.1f,
            -0.5f, 0.8f, 0f, 0.1f,
            -0.5f, -0.8f, 0f, 0.9f
    };

    public Table() {
        vertextArray = new VertextArray(VERTEX_DATA);
    }

    //绑定纹理渲染程序
    public void bindData(TextureShaderProgram textureProgram) {

        vertextArray.setVertexAttribPointer(0,
                textureProgram.getPositionAttributeLocation(),
                POSITION_COMPONENT_COUNT,
                STRIDE);

        vertextArray.setVertexAttribPointer(
                POSITION_COMPONENT_COUNT,
                textureProgram.getTextureCoordinatesAttributeLocation(),
                TEXTURE_COORDINATES_COMPONNET_COUNT,
                STRIDE);
    }

    public void draw() {
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    }


}
