//attribute 就是通过外界将颜色放进opengl着色器中的方式
attribute vec4 a_Position ;
attribute vec4 a_Color ;
varying vec4 v_Color;
uniform mat4 u_Matrix;
void main(){
    v_Color = a_Color;
    gl_Position = a_Position * u_Matrix;
    gl_PointSize = 10.0;
}