
uniform vec4 u_Color;
varying vec2 v_TextureCoordinates;
uniform sampler2D  u_TextureUnit;

void main() {

        gl_FragColor = texture2D(u_TextureUnit , v_TextureCoordinates);
}
