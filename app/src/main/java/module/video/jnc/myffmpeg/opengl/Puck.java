package module.video.jnc.myffmpeg.opengl;

import java.util.List;

/**
 * Created by Administrator on 2017/12/18/018.
 */

public class Puck {

    private static final int POSITION_COMPONENT_COUNT = 3;
    public final float radius , height ;
    private final VertextArray vertexArray;
    private final List<ObjectBuilder.DrawCommand> drawList;

    public Puck(float radius , float height , int numPointsAroundPuck){
        ObjectBuilder.GeneratedData generatedData = ObjectBuilder.createPuck(
                new Geometry.Cylinder(new Geometry.Point(0f , 0f , 0f ) , radius ,height) , numPointsAroundPuck);
        this.radius = radius;
        this.height = height;
        vertexArray = new VertextArray(generatedData.vertexData);
        drawList = generatedData.drawList;
    }

    public void bindData(ColorShaderProgram colorShaderProgram){
        vertexArray.setVertexAttribPointer(0 , colorShaderProgram.getPositionAttributeLocation() , POSITION_COMPONENT_COUNT , 0);
    }

    public void draw(){
        for(ObjectBuilder.DrawCommand drawCommand : drawList){
            drawCommand.draw();
        }
    }



}
