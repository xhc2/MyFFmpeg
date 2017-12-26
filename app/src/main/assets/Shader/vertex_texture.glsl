uniform mat4 textureTransform;
attribute vec2 inputTextureCoordinate;
attribute vec4 position;
varying   vec2 textureCoordinate;

 void main() {
     gl_Position = position;
     textureCoordinate = inputTextureCoordinate;
 }