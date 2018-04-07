

attribute vec4 a_Position;
attribute vec2 a_TextureCoordinates;
//插值器，用于传递给片段着色器
varying vec2 v_TextureCoordinates;


void main() {

    v_TextureCoordinates = a_TextureCoordinates;
    gl_Position = a_Position;
}
