package module.video.jnc.myffmpeg.opengl;

/**
 * Created by xhc on 2017/12/11.
 */
import android.util.Log;

import static android.opengl.GLES20.GL_COMPILE_STATUS;
import static android.opengl.GLES20.GL_LINK_STATUS;
import static android.opengl.GLES20.GL_VALIDATE_STATUS;
import static android.opengl.GLES20.glAttachShader;
import static android.opengl.GLES20.glCompileShader;
import static android.opengl.GLES20.glCreateProgram;
import static android.opengl.GLES20.glCreateShader;
import static android.opengl.GLES20.GL_FRAGMENT_SHADER;
import static android.opengl.GLES20.GL_VERTEX_SHADER;
import static android.opengl.GLES20.glDeleteProgram;
import static android.opengl.GLES20.glDeleteShader;
import static android.opengl.GLES20.glGetProgramInfoLog;
import static android.opengl.GLES20.glGetProgramiv;
import static android.opengl.GLES20.glGetShaderInfoLog;
import static android.opengl.GLES20.glGetShaderiv;
import static android.opengl.GLES20.glLinkProgram;
import static android.opengl.GLES20.glShaderSource;
import static android.opengl.GLES20.glValidateProgram;

public class ShaderHelper {

    public static final String TAG = "ShaderHelper";

    public static int compileVertextShader(String shaderCode){
        return compileShader(GL_VERTEX_SHADER , shaderCode);
    }

    public static int compileFragmentShader(String shaderCode){
        return compileShader(GL_FRAGMENT_SHADER , shaderCode);
    }

    private static int compileShader(int type , String shaderCode){
        final int shaderObjuectId = glCreateShader(type);
        if(shaderObjuectId == 0){
            Log.e("xhc" , " create new shader faild ");
            return 0;
        }
        glShaderSource(shaderObjuectId , shaderCode);
        glCompileShader(shaderObjuectId);
        final int[] compileStatus = new int[1];
        glGetShaderiv(shaderObjuectId , GL_COMPILE_STATUS , compileStatus , 0);
        Log.e("xhc" , " compile result : "+shaderCode +" "+glGetShaderInfoLog(shaderObjuectId));
        if(compileStatus[0] == 0){
            glDeleteShader(shaderObjuectId);
            Log.e("xhc" , " compile of shader faild ");
            return 0;
        }
        return shaderObjuectId;
    }

    public static int linkProgram(int vertexShaderId , int fragmentShaderId){
        final int programObjectId = glCreateProgram();
        if(programObjectId == 0){
            Log.e("xhc" , "create program faild ");
            return 0;
        }
        glAttachShader(programObjectId , vertexShaderId);
        glAttachShader(programObjectId , fragmentShaderId);
        glLinkProgram(programObjectId);
        final int[] linkStatus = new int[1];
        glGetProgramiv(programObjectId , GL_LINK_STATUS , linkStatus , 0);
        Log.e("xhc" , " result link progrom : \n"+glGetProgramInfoLog(programObjectId));
        if(linkStatus[0] == 0){
            glDeleteProgram(programObjectId);
            Log.e("xhc" , " program link faild !");
            return 0;
        }
        return programObjectId;
    }


    /**
     * 判断写的程序是否有效
     * @param programObjectId
     * @return
     */
    public static boolean validatePrograme(int programObjectId){
        glValidateProgram(programObjectId);
        final int[] validateStatus = new int[1];
        glGetProgramiv(programObjectId , GL_VALIDATE_STATUS , validateStatus , 0);
        Log.e("xhc" , " validateing programe : \n"+validateStatus[0]+" log : "+glGetProgramInfoLog(programObjectId));

        return validateStatus[0] != 0;
    }

    public static int buildProgram(String vertexShaderSource, String fragmentShaderSource){

        int program ;
        int vertexShader = compileVertextShader(vertexShaderSource);
        int fragmentShader = compileFragmentShader(fragmentShaderSource);
        program = linkProgram(vertexShader , fragmentShader);
        //验证程序是否可用
        validatePrograme(program);
        return program;
    }

}
