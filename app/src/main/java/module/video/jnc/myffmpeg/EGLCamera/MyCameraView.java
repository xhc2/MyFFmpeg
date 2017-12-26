package module.video.jnc.myffmpeg.EGLCamera;

import android.content.Context;
import android.graphics.Shader;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.opengl.GLES20;
import android.util.Log;

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
//        private float[] mCameraMatrix  = new float[16];
//        private float[] mMVPMatrix     = new float[16];
//        private float[] mTempMatrix    = new float[16];
        private final float[] modelMatrix = new float[16];

        private float[] mPosCoordinate = {-1, -1, -1, 1, 1, -1, 1, 1};
        private float[] mTexCoordinate = {0, 1, 1, 1, 0, 0, 1, 0};

        private FloatBuffer mPosBuffer;
        private FloatBuffer mTexBuffer;

        MyRender(Context context){
            this.context = context;

            //单位矩阵乘以任何矩阵都是得到以前的矩阵
//            Matrix.setIdentityM(mProjectMatrix, 0);
//            Matrix.setIdentityM(mCameraMatrix, 0);
//            Matrix.setIdentityM(mMVPMatrix, 0);
//            Matrix.setIdentityM(mTempMatrix, 0);

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

        @Override
        public void onSurfaceChanged(GL10 gl10, int width, int height) {
            GLES20.glViewport(0, 0, width, height);
            float ratio = (float)width/height;
            //正交投影
            MatrixHelper.perspectiveM(mProjectMatrix, 45, (float) width / (float) height, 1f, 10f);
//            Matrix.orthoM(mProjectMatrix,0,-1,1,-ratio,ratio,1,7);// 3和7代表远近视点与眼睛的距离，非坐标点
//            Matrix.setLookAtM(mCameraMatrix, 0, 0, 0, 3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);// 3代表眼睛的坐标点
//            Matrix.rotateM(modelMatrix , 0 , -60f , 1f , 0f , 0f);
//            Matrix.multiplyMM(temp, 0, projectionMatrix, 0, modelMatrix, 0);
//            Matrix.multiplyMM(mMVPMatrix, 0, mProjectMatrix, 0, mCameraMatrix, 0);

        }

        @Override
        public void onDrawFrame(GL10 gl10) {
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            mCameraTexture.updateTexImage();
            GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mProjectMatrix, 0);
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
    }

}
