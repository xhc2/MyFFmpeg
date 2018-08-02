package module.video.jnc.myffmpeg.EGLCamera;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Shader;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.opengl.Matrix;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import module.video.jnc.myffmpeg.R;
import module.video.jnc.myffmpeg.opengl.MatrixHelper;
import module.video.jnc.myffmpeg.opengl.ShaderHelper;
import module.video.jnc.myffmpeg.opengl.TextResourceReader;

/**
 * Created by xhc on 2017/12/26.
 * 在相机预览界面需要做
 * 1.裁剪，旋转、水印、滤镜，
 * http://blog.csdn.net/oshunz/article/details/50537631滤镜
 */

public class MyCameraView extends GLSurfaceView implements SurfaceTexture.OnFrameAvailableListener {


    public MyCameraView(Context context) {
        super(context);
        setEGLContextClientVersion(2);
        setRenderer(new MyRender(context));
        setRenderMode(RENDERMODE_WHEN_DIRTY);
    }


    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        requestRender();
    }

    class MyRender implements Renderer{

        private Context context;
        private int mProgram;
        private CameraManeger mCameraManeger;
        private SurfaceTexture mCameraTexture;

        private int uPosHandle;
        private int aTexHandle;
        private int mMVPMatrixHandle;

        private float[] mProjectMatrix = new float[16];
        private float[] viewMatrix  = new float[16];
        private float[] viewProjectionMatrix     = new float[16];
//        private float[] mTempMatrix    = new float[16];
        private final float[] modelMatrix = new float[16];
        //使用num来控制脸的胖瘦，就是纹理坐标和opengl的坐标的对应。在《oepngl es应用开发实践指南android》第七章有关于纹理的讲述
        private final float num = 0.7f;
        private float[] mPosCoordinate = {-num, -1, -num, 1, num, -1, num, 1};
        private float[] mTexCoordinate = {0, 1, 1, 1, 0, 0, 1, 0};

        private FloatBuffer mPosBuffer;
        private FloatBuffer mTexBuffer;

        MyRender(Context context){
            this.context = context;
            //单位矩阵乘以任何矩阵都是得到以前的矩阵
//            Matrix.setIdentityM(mProjectMatrix, 0);
//            Matrix.setIdentityM(mCameraMatrix, 0);
            mCameraManeger = new CameraManeger();
        }



        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            GLES20.glClearColor(0.5f, 0.0f, 0.0f, 0.0f);

            String cameraFragment = TextResourceReader.readTextFileFromResource(context , R.raw.camera_fragment_shader);
            String cameraVertex = TextResourceReader.readTextFileFromResource(context , R.raw.camera_vertex_shader);
            mProgram = ShaderHelper.buildProgram(cameraVertex , cameraFragment);
            GLES20.glUseProgram(mProgram);

            createAndBindVideoTexture();

            mCameraManeger.OpenCamera(mCameraTexture);

            uPosHandle = GLES20.glGetAttribLocation (mProgram, "vPosition");
            aTexHandle  = GLES20.glGetAttribLocation (mProgram, "inputTextureCoordinate");
            mMVPMatrixHandle  = GLES20.glGetUniformLocation (mProgram, "u_Matrix");

            mPosBuffer = createFloatBuffer(mPosCoordinate);
            mTexBuffer = createFloatBuffer(mTexCoordinate);

            GLES20.glVertexAttribPointer(uPosHandle, 2, GLES20.GL_FLOAT, false, 0, mPosBuffer);
            GLES20.glVertexAttribPointer(aTexHandle, 2, GLES20.GL_FLOAT, false, 0, mTexBuffer);

            GLES20.glEnableVertexAttribArray(uPosHandle);
            GLES20.glEnableVertexAttribArray(aTexHandle);
            GLES20.glEnableVertexAttribArray(mMVPMatrixHandle);
        }

        private FloatBuffer createFloatBuffer(float[] buffer){
            FloatBuffer fb = ByteBuffer.allocateDirect(buffer.length * 4)
                    .order(ByteOrder.nativeOrder())
                    .asFloatBuffer();
            fb.put(buffer);
            fb.position(0);
            return fb;
        }

        /**
         * 在进行三维空间的投影的时候，会导致变形，要么拉长，要么变胖。不知道怎么回事先放着
         */
        @Override
        public void onSurfaceChanged(GL10 gl10, int width, int height) {
            GLES20.glViewport(0, 0, width, height);
            Matrix.setIdentityM(viewProjectionMatrix, 0);
            Matrix.setIdentityM(viewMatrix, 0);
            Matrix.setIdentityM(modelMatrix, 0);
            float ratio = (float)width/height;
            /**
             * 正交投影,这个视椎体从z值-1位置开始，到-10的位置结束
             * 比如一个90度的视野，焦距是1/tan(90/2) 也就是1
             * 所以把眼睛的位置放在焦点上，然后看着视频的正中心，就是刚好铺满整个手机屏幕
             */
            MatrixHelper.perspectiveM(mProjectMatrix, 90, ratio, 1f, 10f);
//            3和7代表远近视点与眼睛的距离，非坐标点
//            Matrix.orthoM(mProjectMatrix,0,-1,1,-ratio,ratio,3,7);

            Matrix.setLookAtM(viewMatrix, 0, 0, 0, -1.5f,
                    0f, 0f, -2.5f,
                    0f, 1.0f, 0.0f);

//            Matrix.rotateM(modelMatrix , 0 , -60f , 1f , 0f , 0f);

            Matrix.translateM(modelMatrix , 0 , 0f , 0f , -2.5f);
            final float[] temp = new float[16];
            Matrix.multiplyMM(temp, 0, mProjectMatrix, 0, modelMatrix, 0);

            System.arraycopy(temp, 0, mProjectMatrix, 0, temp.length);
        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            Matrix.multiplyMM(viewProjectionMatrix, 0, mProjectMatrix, 0, viewMatrix, 0);
            GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, viewProjectionMatrix, 0);
            mCameraTexture.updateTexImage();
            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, mPosCoordinate.length / 2);
        }


        private void createAndBindVideoTexture() {

            int[] texture = new int[1];
            GLES20.glGenTextures(1, texture, 0);
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture[0]);

            GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MIN_FILTER,GL10.GL_LINEAR);
            GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE);

            mCameraTexture = new SurfaceTexture(texture[0]);
            mCameraTexture.setOnFrameAvailableListener(MyCameraView.this);
        }


        //加载图片作为纹理
        private void createAndBindWaterTexture(){
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(),R.mipmap.air_hockey_surface);
            int[] texture = new int[1];
            GLES20.glGenTextures(1, texture, 0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texture[0]);
            // Set filtering
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
            // Load the bitmap into the bound texture.
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
        }

    }
}
