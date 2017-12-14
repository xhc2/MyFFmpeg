package module.video.jnc.myffmpeg.opengl;


import android.util.FloatMath;

/**
 * Created by xhc on 2017/12/14.
 */

public class ObjectBuilder {

    private static final int FLOATS_PER_VERTEX = 3;
    private final float[] vertexData;
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
//        builder.
//                builder.
        return builder.build();

    }

    private void appendCircle(Geometry.Circle circle , int numPoints){
        vertexData[offset ++] = circle.center.x;
        vertexData[offset ++] = circle.center.y;
        vertexData[offset ++] = circle.center.z;
        for(int i = 0 ;i <= numPoints ; i++ ){
            float angleInRadinans = ((float) i / (float) numPoints) * ((float)Math.PI * 2f);
            vertexData[offset++] = circle.center.x + circle.radius * (float)Math.cos(angleInRadinans);
            vertexData[offset++] = circle.center.y;
            vertexData[offset++] = circle.center.z + circle.radius * (float)Math.sin(angleInRadinans);

        }
    }
}
