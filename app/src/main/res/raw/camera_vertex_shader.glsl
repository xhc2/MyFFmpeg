//vec4有四个分量，x,y,z,w
attribute vec4 vPosition;
attribute vec2 inputTextureCoordinate;
varying vec2 textureCoordinate;
uniform mat4 u_Matrix;
void main() {
//gl_Position open gl 会把gl_position中的存储的值作为当前顶点的最终位置，并把这些顶点组装成点，直线，三角形
    gl_Position = /*u_Matrix  * */vPosition;
    textureCoordinate = inputTextureCoordinate;
}
