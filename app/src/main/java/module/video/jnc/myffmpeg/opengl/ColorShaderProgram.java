package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform1i;
import static android.opengl.GLES20.glUniformMatrix4fv;
import module.video.jnc.myffmpeg.R;

/**
 * Created by xhc on 2017/12/13.
 */

public class ColorShaderProgram extends ShaderProgram{

    private final int uMatrixLocation;
    private final int aPositionLocation;
    private final int aColorLocation;


    public ColorShaderProgram(Context context ) {
        super(context, R.raw.simple_vertex_shader , R.raw.simple_fragment_shader);
        uMatrixLocation = glGetUniformLocation(program , U_MATRIX);
        aPositionLocation = glGetAttribLocation(program , A_POSITION);
        aColorLocation = glGetAttribLocation(program , A_COLOR);

    }

    public void setUniforms(float[] matrix){
        glUniformMatrix4fv(uMatrixLocation , 1 , false, matrix , 0);
    }

    public int getPositionAttributeLocation(){
        return aPositionLocation;
    }

    public int getColorAttributeLocation(){
        return aColorLocation;
    }

}
