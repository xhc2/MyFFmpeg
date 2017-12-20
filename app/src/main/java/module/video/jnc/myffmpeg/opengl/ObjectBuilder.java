package module.video.jnc.myffmpeg.opengl;


import android.util.FloatMath;

import java.util.ArrayList;
import java.util.List;

import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.GL_TRIANGLE_STRIP;
import static android.opengl.GLES20.glDrawArrays;

/**
 * Created by xhc on 2017/12/14.
 */

public class ObjectBuilder {

    private static final int FLOATS_PER_VERTEX = 3;
    private final float[] vertexData;
    private final List<DrawCommand> drawList = new ArrayList<>();

    private int offset = 0;

    private ObjectBuilder(int sizeInvertices){
        vertexData = new float[sizeInvertices * FLOATS_PER_VERTEX];

    }

    //计算圆柱体顶部数量的方法
    private static int sizeOfCircleInvertices(int numPoints){
        return 1 + (numPoints + 1);
    }

    //计算圆柱体侧面顶点的数量
    private static int sizeOfOpenCylinderInVertices(int numPoints){
        return (numPoints + 1) * 2;
    }

    public static GeneratedData createPuck(Geometry.Cylinder puck , int numPoints){
        int size = sizeOfCircleInvertices(numPoints) + sizeOfOpenCylinderInVertices(numPoints);
        ObjectBuilder builder = new ObjectBuilder(size);
        Geometry.Circle puckTop = new Geometry.Circle(puck.center.translateY(puck.height / 2f) , puck.radius);
        builder.appendCircle(puckTop , numPoints);
        builder.appendOpenCylinder(puck , numPoints);
        return builder.build();

    }


    //根据基础几何图形，圆形等。生成锤子等
    public static GeneratedData createMallet(Geometry.Point center , float radius , float height , int numPoints){
        int size = sizeOfCircleInvertices(numPoints) *2 + sizeOfOpenCylinderInVertices(numPoints) * 2;
        ObjectBuilder builder = new ObjectBuilder(size);
        float baseHeight = height * 0.25f;
        Geometry.Circle baseCircle = new Geometry.Circle(center.translateY(-baseHeight) , radius);

        Geometry.Cylinder baseCylinder = new Geometry.Cylinder(baseCircle.center.translateY(-baseHeight / 2f) , radius , baseHeight);
        builder.appendCircle(baseCircle, numPoints);
        builder.appendOpenCylinder(baseCylinder, numPoints);

        float handleHeight = height * 0.75f;
        float handleRadius = radius / 3f;
        Geometry.Circle handleCircle = new Geometry.Circle(center.translateY(height * 0.5f), handleRadius);
        Geometry.Cylinder handleCylinder = new Geometry.Cylinder(handleCircle.center.translateY(-handleHeight / 2f) , handleRadius , handleHeight);

        builder.appendCircle(handleCircle , numPoints);
        builder.appendOpenCylinder(handleCylinder , numPoints);
        return builder.build();
    }

    //生成对应的点
    private void appendOpenCylinder(Geometry.Cylinder cylinder , int numPoints){
        final int startVertex = offset / FLOATS_PER_VERTEX ;
        final int numVertices = sizeOfOpenCylinderInVertices(numPoints);
        final float yStart = cylinder.center.y - (cylinder.height / 2f);
        final float yEnd = cylinder.center.y + (cylinder.height / 2f );

        for(int i = 0  ; i <= numPoints ; i++){
            float angleInRadins = ((float) i / (float)numPoints) * ((float)Math.PI * 2f);
            float xPosition = cylinder.center.z + cylinder.radius * (float)Math.cos(angleInRadins);
            float zPosition = cylinder.center.z + cylinder.radius * (float)Math.sin(angleInRadins);
            vertexData[offset++] = xPosition;
            vertexData[offset++] = yStart;
            vertexData[offset++] = zPosition;

            vertexData[offset++] = xPosition;
            vertexData[offset++] = yEnd;
            vertexData[offset++] = zPosition;
        }

        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                //绘制三角形带
                glDrawArrays(GL_TRIANGLE_STRIP , startVertex , numVertices);
            }
        });
    }

    //生成对应的点
    private void appendCircle(Geometry.Circle circle , int numPoints){
         final int startVertex = offset / FLOATS_PER_VERTEX ;
         final int numVertices = sizeOfOpenCylinderInVertices(numPoints);
        vertexData[offset ++] = circle.center.x;
        vertexData[offset ++] = circle.center.y;
        vertexData[offset ++] = circle.center.z;
        for(int i = 0 ;i <= numPoints ; i++ ){
            float angleInRadinans = ((float) i / (float) numPoints) * ((float)Math.PI * 2f);
            vertexData[offset++] = circle.center.x + circle.radius * (float)Math.cos(angleInRadinans);
            vertexData[offset++] = circle.center.y;
            vertexData[offset++] = circle.center.z + circle.radius * (float)Math.sin(angleInRadinans);
        }
        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                glDrawArrays(GL_TRIANGLE_FAN , startVertex , numVertices);
            }
        });
    }

    private GeneratedData build(){
        return  new GeneratedData(vertexData , drawList);
    }

    static class GeneratedData{
        final float[] vertexData;
        final List<DrawCommand> drawList;

        GeneratedData(float[] vertexData , List<DrawCommand> drawList){
            this.vertexData = vertexData;
            this.drawList = drawList;
        }
    }
    public  interface DrawCommand{
        void draw();
    }

}
