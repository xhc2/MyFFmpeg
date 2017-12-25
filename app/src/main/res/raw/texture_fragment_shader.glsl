
precision mediump float ;
uniform sampler2D u_TextureUnit;
varying vec2 v_TextureCoordinates;

/**
为了把纹理绘制到一个物体上，
oepngl会为每个片段都调用片段着色器，
并且每个调用都接收v_TextureCoordinates的纹理坐标
片段着色器也通过u_TextureUnit接收实际的纹理数据，
u_TextureUnit被定义为一个sampler2D，
这个变量类型指的是一个二维纹理数据的数组
被插值的纹理坐标和纹理数据被传递给着色器texture2D（）；
*/
void main() {
     gl_FragColor = texture2D(u_TextureUnit , v_TextureCoordinates);
}
