/**
    顶点着色器
*/

attribute vec4 a_Position;
//有两个分量，s坐标，和t坐标，所以是vec2
attribute vec2 a_TextureCoordinates;
//varying 被插值
varying vec2 v_TextureCoordinates;

void main() {
    v_TextureCoordinates = a_TextureCoordinates;
    gl_Position = a_Position;
}
