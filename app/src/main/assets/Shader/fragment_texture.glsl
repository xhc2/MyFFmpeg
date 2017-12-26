#extension GL_OES_EGL_image_external : require
precision mediump float;
uniform samplerExternalOES videoTex;
varying vec2 textureCoordinate;

void main() {
   gl_FragColor = texture2D(videoTex, textureCoordinate);
}
