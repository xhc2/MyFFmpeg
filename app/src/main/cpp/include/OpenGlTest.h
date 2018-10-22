//
// Created by Administrator on 2018/10/17/017.
//

#ifndef MYFFMPEG_OPENGLTEST_H
#define MYFFMPEG_OPENGLTEST_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>


//顶点着色器glsl,这是define的单行定义 #x = "x"
#define GET_STR(x) #x
static const char *vertexShader2 = GET_STR(
        attribute
        vec4 aPosition; //顶点坐标
        attribute
        vec2 aTexCoord; //材质顶点坐标
        varying
        vec2 vTexCoord;   //输出的材质坐标
        void main() {
            vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
            gl_Position = aPosition;
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragYUV420P2 = GET_STR(
        precision
        mediump float;    //精度
        varying
        vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform
        sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);


class OpenGlTest{

private:
    ANativeWindow *win;
    char *path ;
    GLuint InitShader(const char *code, GLint type);
    int showYuv(uint8_t *buf_y, uint8_t *buf_u, uint8_t *buf_v);
    int init_opengl();
    int outWidth ;
    int outHeight;
    EGLSurface winsurface;
    EGLDisplay display;
    GLuint vsh;
    GLuint fsh;
    GLuint program;
    GLuint texts[3] ;
    GLuint apos;
    GLuint atex;
    EGLContext context;
    bool initOpenglFlag;


public :
    void start();
    OpenGlTest(const char* path , ANativeWindow *win);
    ~OpenGlTest();

};

#endif //MYFFMPEG_OPENGLTEST_H
