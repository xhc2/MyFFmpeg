package module.video.jnc.myffmpeg.opengl;
import java.util.List;

import static android.opengl.GLES20.GL_POINTS;
import static android.opengl.GLES20.glDrawArrays;
/**
 * Created by xhc on 2017/12/13.
 * 木槌
 */

public class Mallet {

    private static final int POSITION_COMPONENT_COUNT = 3;

    public final float radius ;
    public final float height;

    private final VertextArray vertextArray;
    private final List<ObjectBuilder.DrawCommand> drawList;

    public Mallet(float radius , float height , int numPointsAroundMallet){
        ObjectBuilder.GeneratedData generatedData = ObjectBuilder.createMallet(new Geometry.Point(0f , 0f , 0f ) ,
                radius , height , numPointsAroundMallet);

        this.radius = radius;
        this.height = height;

        vertextArray = new VertextArray(generatedData.vertexData);
        drawList = generatedData.drawList;

    }

    public void bindData(ColorShaderProgram colorShaderProgram){
        vertextArray.setVertexAttribPointer(0 , colorShaderProgram.getPositionAttributeLocation() , POSITION_COMPONENT_COUNT , 0);
    }

    public void draw(){
        for(ObjectBuilder.DrawCommand drawCommand : drawList){
            drawCommand.draw();
        }
    }



//    private static final int POSITION_COMPONENT_COUNT = 2 ;
//    private static final int COLOR_COMPONENT_COUNT = 3;
//    private static final int STRIDE = (POSITION_COMPONENT_COUNT + COLOR_COMPONENT_COUNT)*Constant.BYTES_PER_FLOAT;
//
//    private static final float[] VERTEX_DATA = {
//        // X , Y , R , G , B
//            0f , -0.4f , 0f , 0f , 1f ,
//            0f , 0.4f , 1f , 0f , 0f ,
//    };
//
//    private final VertextArray vertextArray ;
//
//    public Mallet(){
//        vertextArray = new VertextArray(VERTEX_DATA);
//    }
//
//    public void bindData(ColorShaderProgram colorShaderProgram){
//        vertextArray.setVertexAttribPointer(0 , colorShaderProgram.getPositionAttributeLocation() , POSITION_COMPONENT_COUNT , STRIDE);
//        vertextArray.setVertexAttribPointer(POSITION_COMPONENT_COUNT , colorShaderProgram.getColorAttributeLocation() , COLOR_COMPONENT_COUNT , STRIDE);
//    }
//
//    public void draw(){
//        glDrawArrays(GL_POINTS , 0 , 2);
//    }
}
