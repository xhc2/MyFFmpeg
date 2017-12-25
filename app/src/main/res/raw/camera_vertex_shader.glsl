#version 120

attribute vec4 vPosition;
attribute vec2 inputTextureCoordinate;
varying vec2 textureCoordinate;
void main() {
    gl_Position = vPosition;
    textureCoordinate = inputTextureCoordinate;
}
