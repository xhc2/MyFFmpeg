
uniform vec4 u_Color;
varying vec2 v_TextureCoordinates;
//uniform sampler2D  u_TextureUnit;
uniform sampler2D textureY;
uniform sampler2D textureU;
uniform sampler2D textureV;
void main() {
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(textureY , v_TextureCoordinates).a;
    yuv.y = texture2D(textureU , v_TextureCoordinates).a - 0.5;
    yuv.z = texture2D(textureV , v_TextureCoordinates).a - 0.5;
    rgb = mat3(   1 ,        1 ,      1 ,
                  0,        -0.39465 , 2.03210 ,
                  1.13983 , -0.58060 , 0) * yuv;
    gl_FragColor = vec4(rgb , 1);
}
