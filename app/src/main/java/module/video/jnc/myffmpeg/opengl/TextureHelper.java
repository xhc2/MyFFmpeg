package module.video.jnc.myffmpeg.opengl;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLES20.*;
import android.util.Log;

import static android.opengl.GLES20.GL_LINEAR;
import static android.opengl.GLES20.GL_LINEAR_MIPMAP_LINEAR;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES20.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glDeleteTextures;
import static android.opengl.GLES20.glGenTextures;
import static android.opengl.GLES20.glGenerateMipmap;
import static android.opengl.GLES20.glTexImage2D;
import static android.opengl.GLES20.glTexParameteri;
import static android.opengl.GLUtils.texImage2D;

/**
 * Created by xhc on 2017/12/13.
 */

public class TextureHelper {
    public static int loadTexture(Context context , int resourceId){
        final int[] textureObjectIds = new int[1];
        glGenTextures(1,textureObjectIds , 0);
        if(textureObjectIds[0] == 0){
            Log.e("xhc" , "cant open opengl texture object ！");
            return 0;
        }
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources() , resourceId , options);
        if(bitmap == null){
            Log.e("xhc" , "bitmap create faild ");
            glDeleteTextures(1, textureObjectIds , 0);
            return 0;
        }
        //绑定纹理
        glBindTexture(GL_TEXTURE_2D , textureObjectIds[0]);
        /**
         * 设置过滤器GL_TEXTURE_MIN_FILTER指缩小的情况选择GL_LINEAR_MIPMAP_LINEAR说明缩小使用三线性过滤
         * GL_TEXTURE_MAG_FILTER 指放大的情况GL_LINEAR使用双线性过滤
         */

        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
        //加载位图数据到opengl中
        texImage2D(GL_TEXTURE_2D , 0 , bitmap,0);

        bitmap.recycle();
        //生成mip贴图
        glGenerateMipmap(GL_TEXTURE_2D);
        //解除纹理绑定
        glBindTexture(GL_TEXTURE_2D , 0);
        return textureObjectIds[0];
    }


}
