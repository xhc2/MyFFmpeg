#extension GL_OES_EGL_image_external : require
precision mediump float ;
varying vec2 textureCoordinate;
uniform samplerExternalOES s_texture;
uniform vec3 vChangeColor;
uniform sampler2D u_TextureUnit;
void main() {
    gl_FragColor = texture2D(s_texture , textureCoordinate);
}
/**
片段着色器的主要目的就是告诉gpu每个片段的最终颜色是什么，每个片段着色器都会被调用一次
precision mediump float ;定义了浮点数据的默认精度 有其他选择。lowp ， mediump ，highp
*/