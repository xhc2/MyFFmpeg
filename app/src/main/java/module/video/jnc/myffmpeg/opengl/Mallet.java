package module.video.jnc.myffmpeg.opengl;
import static android.opengl.GLES20.GL_POINTS;
import static android.opengl.GLES20.glDrawArrays;
/**
 * Created by xhc on 2017/12/13.
 * 木槌
 */

public class Mallet {
    private static final int POSITION_COMPONENT_COUNT = 2 ;
    private static final int COLOR_COMPONENT_COUNT = 3;
    private static final int STRIDE = (POSITION_COMPONENT_COUNT + COLOR_COMPONENT_COUNT)*Constant.BYTES_PER_FLOAT;

    private static final float[] VERTEX_DATA = {
        // X , Y , R , G , B
            0f , -0.4f , 0f , 0f , 1f ,
            0f , 0.4f , 1f , 0f , 0f ,
    };

    private final VertextArray vertextArray ;

    public Mallet(){
        vertextArray = new VertextArray(VERTEX_DATA);
    }

    public void bindData(ColorShaderProgram colorShaderProgram){
        vertextArray.setVertexAttribPointer(0 , colorShaderProgram.getPositionAttributeLocation() , POSITION_COMPONENT_COUNT , STRIDE);
        vertextArray.setVertexAttribPointer(POSITION_COMPONENT_COUNT , colorShaderProgram.getColorAttributeLocation() , COLOR_COMPONENT_COUNT , STRIDE);
    }

    public void draw(){
        glDrawArrays(GL_POINTS , 0 , 2);
    }
}
