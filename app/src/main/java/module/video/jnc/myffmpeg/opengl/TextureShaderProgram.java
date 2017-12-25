package module.video.jnc.myffmpeg.opengl;

import android.content.Context;

import module.video.jnc.myffmpeg.R;

import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform1i;
import static android.opengl.GLES20.glUniformMatrix4fv;

/**
 * Created by xhc on 2017/12/13.
 * 纹理渲染程序
 */

public class TextureShaderProgram extends ShaderProgram{

    private final int uMatrixLocaktion;
    private final int uTextureUnitLocation;
    private final int aPositionLocation;
    private final int aTextureCoordinatesLocation;

    public TextureShaderProgram(Context context) {
        super(context, R.raw.texture_vertext_shader, R.raw.texture_fragment_shader);
        //找到各个属性的位置
        uMatrixLocaktion = glGetUniformLocation(program , U_MATRIX);
        uTextureUnitLocation = glGetUniformLocation(program , U_TEXTURE_UNIT);
        aPositionLocation = glGetAttribLocation(program , A_POSITION);
        aTextureCoordinatesLocation = glGetAttribLocation(program , A_TEXTURE_COORDINATES);
    }

    public void setUniforms(float[] matrix , int textureId){
        //正交投影
        glUniformMatrix4fv(uMatrixLocaktion , 1 , false , matrix , 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D , textureId);
        glUniform1i(uTextureUnitLocation , 0);
    }

    public int getPositionAttributeLocation(){
        return aPositionLocation;
    }

    public int getTextureCoordinatesAttributeLocation(){
        return aTextureCoordinatesLocation ;
    }

}
