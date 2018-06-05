//
// Created by xhc on 2018/5/10.
//
#include <my_log.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <my_data.h>
#include "gpu_video_sl_audio.h"
#include <thread>
#include <queue>

/**
 * https://www.jianshu.com/p/d5a0ed770b3d
 * 倍速播放，只是单单改变采样率音调会变的很难听。应该不能用这个方法。可以用现成的库 ExoPlayer，sonic什么的
 */

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
using namespace std;
//ffmepg
AVFrame *aframe_gpu;
AVFrame *vframe_gpu;
AVFrame *vframe_gpu_seek;
AVFormatContext *afc_gpu;
int video_index_gpu = -1;
int audio_index_gpu = -1;
AVCodec *videoCode_gpu, *audioCode_gpu;
AVCodecContext *ac_gpu, *vc_gpu;
int outWidth_gpu = 480, outHeight_gpu = 272;
SwrContext *swc_gpu;
//视频总长度
int64_t videoDuration_gpu;

//audio_sl
SLObjectItf engineOpenSL_gpu = NULL;
SLPlayItf iplayer_gpu = NULL;
SLEngineItf eng_gpu = NULL;
SLObjectItf mix_gpu = NULL;
SLObjectItf player_gpu = NULL;
SLAndroidSimpleBufferQueueItf pcmQue_gpu = NULL;
int outFormat_gpu = AV_SAMPLE_FMT_S16;

//other
bool pauseFlag = false;
bool yuvRunFlag_gpu = false;
bool pcmRunFlag_gpu = false;
bool readFrameFlag_gpu = false;
queue<AVPacket *> audioPktQue_gpu;
queue<AVPacket *> videoPktQue_gpu;
queue<MyData> audioFrameQue_gpu;
int64_t apts_gpu = -1;
//视频pts定位
int64_t vpts_gpu = -1;

//用来定位用户定位的位置
int64_t vpts_seek_gpu = -1;
unsigned char *play_audio_buffer = 0;
unsigned char *play_audio_temp = 0;
int maxAudioPacket_gpu = 140;
int maxVideoPacket_gpu = 100;
pthread_t readFrameThread;
pthread_t decodeYuvThread;
pthread_t decodePcm;
pthread_t playAudioDelayThread;
pthread_t jumpToSeekPosThread;
pthread_mutex_t mutex_pthread = PTHREAD_MUTEX_INITIALIZER;

// shader part
bool initOpenglFlag = false;
ANativeWindow *nwin;
EGLSurface winsurface;
EGLDisplay display;
GLuint vsh;
GLuint fsh;
GLuint program;
GLuint texts[3] = {0};
GLuint apos;
GLuint atex;
EGLContext context;
//顶点着色器glsl,这是define的单行定义 #x = "x"
#define GET_STR(x) #x
static const char *vertexShader_gpu = GET_STR(
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
static const char *fragYUV420P_gpu = GET_STR(
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


GLuint InitShader_gpu(const char *code, GLint type) {
    GLuint sh = glCreateShader((GLenum) type);
    if (sh == 0) {
        LOGE("glCreateShader FAILD ");
        return 0;
    }
    //加载shader
    glShaderSource(sh, 1, &code, 0);
    //编译shader
    glCompileShader(sh);
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGE("glCompieshader FAILD ");
        return (GLuint) -1;
    }
    return sh;
}


int init_opengl() {
    if (outHeight_gpu == 0 || outWidth_gpu == 0) {
        LOGE(" outHeight_gpu == 0 || outWidth_gpu == 0 ");
        return RESULT_FAILD;
    }
    initOpenglFlag = true;
    LOGE("  out width %d , out height %d ", outWidth_gpu, outHeight_gpu);
    //获取原始窗口

    //egl display 创建和初始化
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE(" eglGetDisplay FAILD ! ");
        return -1;
    }
    if (EGL_TRUE != eglInitialize(display, 0, 0)) {
        LOGE(" eglInitialize FAILD ! ");
        return -1;
    }
    //窗口配置，输出配置
    EGLConfig config;
    EGLint configNum;
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };
    if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &configNum)) {
        LOGE("eglchooseconfig faild !");
        return -1;
    }
    //创建surface
    winsurface = eglCreateWindowSurface(display, config, nwin, 0);
    if (winsurface == EGL_NO_SURFACE) {
        LOGE(" eglCreateWindowSurface FAILD !");
        return -1;
    }
    //3 context 创建关联的上下文
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    //context 创建关联的 上下文
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext FAILD !");
    }
    if (EGL_TRUE != eglMakeCurrent(display, winsurface, winsurface, context)) {
        LOGE(" eglMakeCurrent faild ! ");
        return -1;
    }


    vsh = InitShader_gpu(vertexShader_gpu, GL_VERTEX_SHADER);
    fsh = InitShader_gpu(fragYUV420P_gpu, GL_FRAGMENT_SHADER);

    program = glCreateProgram();

    if (program == 0) {
        LOGE(" glCreateProgram FAILD ! ");
        return -1;
    }

    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGE(" GLINK PROGRAM faild !");
        return -1;
    }
    glUseProgram(program);

    //加入三维顶点数据 两个三角形组成正方形
    const float vers[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
    };

    apos = (GLuint) glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(apos);

    //传递顶点
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 12, vers);

    //加入材质坐标数据
    const float txts[] = {
            1.0f, 0.0f, //右下
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0, 1.0
    };

    atex = (GLuint) glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(atex);

    glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, 8, txts);

    //材质纹理初始化
    //设置纹理层
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0); //对于纹理第1层
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1); //对于纹理第2层
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2); //对于纹理第3层




    //创建三个纹理
    glGenTextures(3, texts);
    //设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[0]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,           //细节基本 0默认
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图
                 outWidth_gpu, outHeight_gpu, //拉升到全屏
                 0,             //边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图 要与上面一致
                 GL_UNSIGNED_BYTE, //像素的数据类型
                 NULL                    //纹理的数据
    );

    //设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[1]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,           //细节基本 0默认
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图
                 outWidth_gpu / 2,
                 outHeight_gpu / 2, //拉升到全屏
                 0,             //边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图 要与上面一致
                 GL_UNSIGNED_BYTE, //像素的数据类型
                 NULL                    //纹理的数据
    );

    //设置纹理属性
    glBindTexture(GL_TEXTURE_2D, texts[2]);
    //缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,           //细节基本 0默认
                 GL_LUMINANCE,//gpu内部格式 亮度，灰度图
                 outWidth_gpu / 2, outHeight_gpu / 2, //拉升到全屏
                 0,             //边框
                 GL_LUMINANCE,//数据的像素格式 亮度，灰度图 要与上面一致
                 GL_UNSIGNED_BYTE, //像素的数据类型
                 NULL                    //纹理的数据
    );


    LOGE(" INIT shader SUCCESS ");
    return RESULT_SUCCESS;
}



//ffmepg part

int initFFmpeg_gpu(const char *input_path) {

    int result = 0;
    av_register_all();
    avcodec_register_all();

    aframe_gpu = av_frame_alloc();
    vframe_gpu = av_frame_alloc();
    vframe_gpu_seek = av_frame_alloc();
    LOGE(" input path %s ", input_path);
    result = avformat_open_input(&afc_gpu, input_path, 0, 0);
    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_open_input FAILD !");
        return RESULT_FAILD;
    }

    result = avformat_find_stream_info(afc_gpu, 0);


    if (result != 0) {
        LOGE("avformat_open_input failed!:%s", av_err2str(result));
        LOGE("avformat_find_stream_info FAILD !");
        return RESULT_FAILD;
    }

    videoDuration_gpu = afc_gpu->duration / (AV_TIME_BASE / 1000);

    LOGE(" video duration %lld ", videoDuration_gpu);

    for (int i = 0; i < afc_gpu->nb_streams; ++i) {
        AVStream *avStream = afc_gpu->streams[i];
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            video_index_gpu = i;

            LOGE("VIDEO WIDTH %d , HEIGHT %d , format %d , fps %f ", avStream->codecpar->width,
                 avStream->codecpar->height, avStream->codecpar->format,
                 av_q2d(avStream->avg_frame_rate));

            videoCode_gpu = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!videoCode_gpu) {
                LOGE("VIDEO avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audio_index_gpu = i;
            LOGE("audio samplerate %d ", avStream->codecpar->sample_rate);
            audioCode_gpu = avcodec_find_decoder(avStream->codecpar->codec_id);

            if (!audioCode_gpu) {
                LOGE("audio avcodec_find_decoder FAILD!");
                return RESULT_FAILD;
            }
        }
    }

    ac_gpu = avcodec_alloc_context3(audioCode_gpu);
    if (!ac_gpu) {
        LOGE("ac_gpu AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    vc_gpu = avcodec_alloc_context3(videoCode_gpu);
    if (!vc_gpu) {
        LOGE("vc_gpu AVCodecContext FAILD ! ");
        return RESULT_FAILD;
    }

    //将codec中的参数放进accodeccontext
    avcodec_parameters_to_context(vc_gpu, afc_gpu->streams[video_index_gpu]->codecpar);
    avcodec_parameters_to_context(ac_gpu, afc_gpu->streams[audio_index_gpu]->codecpar);

    vc_gpu->thread_count = 4;
    ac_gpu->thread_count = 4;

    result = avcodec_open2(vc_gpu, NULL, NULL);
    if (result != 0) {
        LOGE("vc_gpu avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    result = avcodec_open2(ac_gpu, NULL, NULL);
    if (result != 0) {
        LOGE("ac_gpu avcodec_open2 Faild !");
        return RESULT_FAILD;
    }

    outWidth_gpu = vc_gpu->width;
    outHeight_gpu = vc_gpu->height;

    LOGE("outwidth %d , outheight %d ", outWidth_gpu, outHeight_gpu);

    //音频重采样上下文初始化 , AV_SAMPLE_FMT_S16 格式的单声道
    swc_gpu = swr_alloc_set_opts(NULL,
                                 av_get_default_channel_layout(1),
                                 AV_SAMPLE_FMT_S16, ac_gpu->sample_rate,

                                 av_get_default_channel_layout(ac_gpu->channels),
                                 ac_gpu->sample_fmt, ac_gpu->sample_rate,
                                 0, 0);
    result = swr_init(swc_gpu);
    play_audio_buffer = new unsigned char[2 * 1024];
    play_audio_temp = new unsigned char[2 * 1024];
    if (result < 0) {
        LOGE(" swr_init FAILD !");
        return RESULT_FAILD;
    }
    LOGE(" init ffmpeg success ! ");
    return RESULT_SUCCESS;
}

// audio part
SLEngineItf createOpenSL_gpu() {
    SLresult re = 0;
    SLEngineItf en = NULL;

    re = slCreateEngine(&engineOpenSL_gpu, 0, 0, 0, 0, 0);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("slCreateEngine FAILD ");
        return NULL;
    }

    re = (*engineOpenSL_gpu)->Realize(engineOpenSL_gpu, SL_BOOLEAN_FALSE);

    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return NULL;
    }

    re = (*engineOpenSL_gpu)->GetInterface(engineOpenSL_gpu, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface FAILD ");
        return NULL;
    }

    return en;
}

void pcmCallBack_gpu(SLAndroidSimpleBufferQueueItf bf, void *context) {
    if (!audioFrameQue_gpu.empty()) {
        MyData myData;
        myData = audioFrameQue_gpu.front();
        audioFrameQue_gpu.pop();
        memcpy(play_audio_buffer, myData.data, myData.size);
        (*bf)->Enqueue(bf, play_audio_buffer, myData.size);
        apts_gpu = myData.pts;
        free(myData.data);
    }
}

int initAudio_gpu() {
    //创建引擎
    eng_gpu = createOpenSL_gpu();
    if (!eng_gpu) {
        LOGE("createSL FAILD ");
    }

    //2.创建混音器
    mix_gpu = NULL;
    SLresult re = 0;
    re = (*eng_gpu)->CreateOutputMix(eng_gpu, &mix_gpu, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix FAILD ");
        return RESULT_FAILD;
    }
    re = (*mix_gpu)->Realize(mix_gpu, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("Realize FAILD ");
        return RESULT_FAILD;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix_gpu};
    SLDataSink audioSink = {&outmix, 0};

    //配置音频信息
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};

    SLuint32 kz = (SLuint32)(48000000 * 2);

    //音频格式
    SLDataFormat_PCM pcm_ = {
            SL_DATAFORMAT_PCM,
            1,//    声道数
            kz ,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm_};

    //创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng_gpu)->CreateAudioPlayer(eng_gpu, &player_gpu, &ds, &audioSink,
                                       sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateAudioPlayer FAILD ");
        return RESULT_FAILD;
    }
    (*player_gpu)->Realize(player_gpu, SL_BOOLEAN_FALSE);
    re = (*player_gpu)->GetInterface(player_gpu, SL_IID_PLAY, &iplayer_gpu);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_PLAY FAILD ");
        return RESULT_FAILD;
    }
    re = (*player_gpu)->GetInterface(player_gpu, SL_IID_BUFFERQUEUE, &pcmQue_gpu);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("GetInterface SL_IID_BUFFERQUEUE FAILD ");
        return -1;
    }

    (*pcmQue_gpu)->RegisterCallback(pcmQue_gpu, pcmCallBack_gpu, 0);

    LOGE(" OpenSles init SUCCESS ");
    return RESULT_SUCCESS;
}


void ThreadSleep_gpu(int mis) {
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

void *readFrame_gpu(void *arg) {
    int result = 0;
    while (readFrameFlag_gpu) {
//        LOGE(" audioPktQue.size() %d , videoPktQue.size() %d", audioPktQue_gpu.size(),
//             videoPktQue_gpu.size());

        if (pauseFlag) {
            ThreadSleep_gpu(500);
            continue;
        }

        if (audioPktQue_gpu.size() >= maxAudioPacket_gpu ||
            videoPktQue_gpu.size() >= maxVideoPacket_gpu) {
            //控制缓冲大小
            ThreadSleep_gpu(2);
            continue;
        }

        AVPacket *pkt_ = av_packet_alloc();
        result = av_read_frame(afc_gpu, pkt_);
        if (result < 0) {
            ThreadSleep_gpu(2);
            av_packet_free(&pkt_);
            continue;
        }
        if (pkt_->stream_index == audio_index_gpu) {
//            pkt_->pts = (int64_t) (pkt_->pts * (1000 *
//                                                av_q2d(afc_gpu->streams[pkt_->stream_index]->time_base)));
//            pkt_->dts = (int64_t) (pkt_->dts * (1000 *
//                                                av_q2d(afc_gpu->streams[pkt_->stream_index]->time_base)));
            audioPktQue_gpu.push(pkt_);
        } else if (pkt_->stream_index == video_index_gpu) {

//            pkt_->pts = (int64_t) (pkt_->pts * (1000 *
//                                                av_q2d(afc_gpu->streams[pkt_->stream_index]->time_base)));
//            pkt_->dts = (int64_t) (pkt_->dts * (1000 *
//                                                av_q2d(afc_gpu->streams[pkt_->stream_index]->time_base)));
//这边read一直是正确的，问题不知道出在哪里了。视频解码那边偶尔出问题。但是一般会是5帧左右。
//            LOGE(" READ PTS %lld ", pkt_->pts);

            videoPktQue_gpu.push(pkt_);
        } else {
            av_packet_free(&pkt_);
        }
    }
    return (void *) RESULT_SUCCESS;
}

// 0 y , 1 u , 2 v
int showYuv(uint8_t *buf_y, uint8_t *buf_u, uint8_t *buf_v) {

    //激活第1层纹理,绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texts[0]);
    //替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, outWidth_gpu, outHeight_gpu, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    buf_y);

    //激活第2层纹理,绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texts[1]);
    //替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, outWidth_gpu / 2, outHeight_gpu / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, buf_u);

    //激活第2层纹理,绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, texts[2]);
    //替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, outWidth_gpu / 2, outHeight_gpu / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, buf_v);

    //三维绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //窗口显示
    eglSwapBuffers(display, winsurface);
    ////纹理的修改和显示


    return RESULT_SUCCESS;
}

int typeICount = 0;

//解码视频数据
void *decodeVideo_gpu(void *arg) {
    int result;
    if (!initOpenglFlag) {
        initOpenglFlag = true;
        init_opengl();
    }

    while (yuvRunFlag_gpu) {
        //测试代码
        if (pauseFlag) {
            ThreadSleep_gpu(50);
            continue;
        }

        if (videoPktQue_gpu.empty()) {
            ThreadSleep_gpu(2);
            continue;
        }
        AVPacket *pck = videoPktQue_gpu.front();
//        LOGE(" pop pck %lld " , pck->pts);
        //音视频同步处理
        int64_t pts = getConvertPts(pck->pts, afc_gpu->streams[pck->stream_index]->time_base);
        LOGE("tong bu apts %lld , vpts %lld " , apts_gpu , pts);
        if (pts > apts_gpu) {
            ThreadSleep_gpu(1);
            continue;
        }

        videoPktQue_gpu.pop();
        if (!pck) {
            LOGE(" video packet null !");
            continue;
        }

        result = avcodec_send_packet(vc_gpu, pck);
        av_packet_free(&pck);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            continue;
        }


        while (true) {
            result = avcodec_receive_frame(vc_gpu, vframe_gpu);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
                break;
            } else if (result < 0) {
                break;
            }
//            if (AV_PICTURE_TYPE_I == vframe_gpu->pict_type) {
//                typeICount++;
//                视频关键帧的pts，分别转换前                 后
//                 I TYPE COUNT 1 ， pts 0                   0
//                 I TYPE COUNT 2 ， pts 38912               3040
//                 I TYPE COUNT 3 ， pts 61952               4840
//                 I TYPE COUNT 4 ， pts 163328              12760
//                 I TYPE COUNT 5 ， pts 233984              18280
//                 I TYPE COUNT 6 ， pts 279040              21800
//                 I TYPE COUNT 7 ， pts 312832              24440
//                LOGE(" I TYPE COUNT %d ， pts %lld ", typeICount,
//                     av_frame_get_best_effort_timestamp(vframe_gpu));
//            }


            vframe_gpu->pts = getConvertPts(vframe_gpu->pts,
                                            afc_gpu->streams[video_index_gpu]->time_base);
            vpts_gpu = vframe_gpu->pts;
            LOGE(" vpts_gpu pts %lld ", vpts_gpu);
//            ThreadSleep_gpu(40);
            showYuv(vframe_gpu->data[0], vframe_gpu->data[1], vframe_gpu->data[2]);
        }
    }
    return (void *) RESULT_SUCCESS;
}

void *decodeAudio_gpu(void *arg) {

    int result = 0;
    int audioCount = 0;
//    int64_t temp_pts = 0;
    while (pcmRunFlag_gpu) {
        if (pauseFlag) {
            ThreadSleep_gpu(50);
            continue;
        }

        if (audioFrameQue_gpu.size() >= maxAudioPacket_gpu || audioPktQue_gpu.empty()) {
            ThreadSleep_gpu(2);
            continue;
        }

        AVPacket *pck = audioPktQue_gpu.front();
        audioPktQue_gpu.pop();
        if (!pck) {
            LOGE(" packet null !");
            continue;
        }
        result = avcodec_send_packet(ac_gpu, pck);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            av_packet_free(&pck);
            continue;
        }
        av_packet_free(&pck);
        while (true) {
            result = avcodec_receive_frame(ac_gpu, aframe_gpu);

            if (result < 0) {
                break;
            }
            audioCount++;
            uint8_t *out[1] = {0};
            out[0] = (uint8_t *) play_audio_temp;
            MyData myData;
            //音频重采样
            swr_convert(swc_gpu, out,
                        aframe_gpu->nb_samples,
                        (const uint8_t **) aframe_gpu->data,
                        aframe_gpu->nb_samples);
            //音频部分需要自己维护一个缓冲区，通过他自己回调的方式处理

            myData.size = av_get_bytes_per_sample((AVSampleFormat) outFormat_gpu) *
                          aframe_gpu->nb_samples;
            myData.data = (char *) malloc(myData.size);;
            memcpy(myData.data, play_audio_temp, myData.size);
            myData.isAudio = true;
            myData.pts = getConvertPts(aframe_gpu->pts  , afc_gpu->streams[audio_index_gpu]->time_base);
            audioFrameQue_gpu.push(myData);

        }

    }
    return (void *) RESULT_SUCCESS;
}


int open_gpu(JNIEnv *env, const char *path, jobject win) {
    int result = RESULT_FAILD;

    nwin = ANativeWindow_fromSurface(env, win);

    result = initFFmpeg_gpu(path);
    if (RESULT_FAILD == result) {
        LOGE(" initFFmpeg_gpu faild");
        return RESULT_FAILD;
    }

    result = initAudio_gpu();
    if (RESULT_FAILD == result) {
        LOGE(" initAudio_gpu faild");
        return RESULT_FAILD;
    }

    //读取帧线程
    readFrameFlag_gpu = true;
    yuvRunFlag_gpu = true;
    pcmRunFlag_gpu = true;


    pthread_create(&readFrameThread, NULL, readFrame_gpu, NULL);
    pthread_create(&decodeYuvThread, NULL, decodeVideo_gpu, NULL);
    pthread_create(&decodePcm, NULL, decodeAudio_gpu, NULL);
    pthread_create(&playAudioDelayThread, NULL, audioPlayDelay_gpu, NULL);

    return RESULT_SUCCESS;
}

//通过标志位全部销毁线程
void stopAllThread() {
    readFrameFlag_gpu = false;
    yuvRunFlag_gpu = false;
    pcmRunFlag_gpu = false;

    void *t;

    pthread_join(readFrameThread, &t);
    pthread_join(decodeYuvThread, &t);
    pthread_join(decodePcm, &t);
    pthread_join(playAudioDelayThread, &t);

    clearAllQue();

}

void *audioPlayDelay_gpu(void *arg) {
    //设置为播放状态,第一次为了保证队列中有数据，所以需要延迟点播放
    pthread_mutex_lock(&mutex_pthread);
    ThreadSleep_gpu(300);
    (*iplayer_gpu)->SetPlayState(iplayer_gpu, SL_PLAYSTATE_PLAYING);
    (*pcmQue_gpu)->Enqueue(pcmQue_gpu, "", 1);
    LOGE(" BUFFER SIZE audio %d ", audioFrameQue_gpu.size());
    pthread_mutex_unlock(&mutex_pthread);
    return (void *) RESULT_SUCCESS;
}

//暂停或者播放
int pause_audio_gpu(bool myPauseFlag) {
    if (iplayer_gpu != NULL) {
        SLresult re = (*iplayer_gpu)->SetPlayState(iplayer_gpu, myPauseFlag ? SL_PLAYSTATE_PAUSED
                                                                            : SL_PLAYSTATE_PLAYING);
        if (re != SL_RESULT_SUCCESS) {
            LOGE("SetPlayState pause FAILD ");
            return -1;
        }
        LOGE("SetPlayState pause success ");
    }
    return 0;
}

int playOrPause_gpu() {
    pauseFlag = !pauseFlag;
    if (pauseFlag) {
        //暂停
        LOGE(" playOrPause_gpu pauseing ");
        pause_audio_gpu(pauseFlag);
    } else {
        //播放
        pthread_create(&playAudioDelayThread, NULL, audioPlayDelay_gpu, NULL);
    }
    return pauseFlag;
}

//就是暂停
int justPause_gpu() {
    pauseFlag = true;
    return 1;
}

int justPlay_gpu() {

    pauseFlag = false;
    //延迟播放音频
    pthread_create(&playAudioDelayThread, NULL, audioPlayDelay_gpu, NULL);
    return RESULT_SUCCESS;
}

int clearAllQue() {
    while (!audioPktQue_gpu.empty()) {
        AVPacket *pck = audioPktQue_gpu.front();
        av_packet_free(&pck);
        audioPktQue_gpu.pop();
    }
    while (!videoPktQue_gpu.empty()) {
        AVPacket *pck = videoPktQue_gpu.front();
        av_packet_free(&pck);
        videoPktQue_gpu.pop();
    }
    while (!audioFrameQue_gpu.empty()) {
        MyData myData = audioFrameQue_gpu.front();
        free(myData.data);
        audioFrameQue_gpu.pop();
    }
    return RESULT_SUCCESS;
}


int getConvertPts(int64_t pts, AVRational time_base) {
    return (int)(pts * av_q2d(time_base) * 1000);
}

//一直读取帧，一直到用户选择的位置
void *jumpToSeekFrame(void *arg) {
    pthread_mutex_lock(&mutex_pthread);
    int result = -1;

    while (vpts_seek_gpu != -1) {

        AVPacket *pkt_ = av_packet_alloc();
        result = av_read_frame(afc_gpu, pkt_);
        if (result < 0 || pkt_->size <= 0) {
            av_packet_free(&pkt_);
            continue;
        }
        if (pkt_->stream_index == audio_index_gpu) {
//            pkt_->pts = (int64_t) (pkt_->pts * (1000 *
//                                                av_q2d(afc_gpu->streams[pkt_->stream_index]->time_base)));
            if (pkt_->pts >= vpts_seek_gpu) {
                audioPktQue_gpu.push(pkt_);
            } else {
                av_packet_free(&pkt_);
            }
            continue;
        }
        result = avcodec_send_packet(vc_gpu, pkt_);
        av_packet_free(&pkt_);
        if (result < 0) {
            LOGE(" SEND PACKET FAILD !");
            continue;
        }

        while (true) {

            result = avcodec_receive_frame(vc_gpu, vframe_gpu_seek);

            if (result < 0) {
                break;
            }
            LOGE("  jumpToSeekFrame  vframe_gpu_seek->pts %lld , vpts_seek_gpu %lld ",
                 av_frame_get_best_effort_timestamp(vframe_gpu_seek),
                 vpts_seek_gpu);

            if (vframe_gpu_seek->pts < vpts_seek_gpu) {
                continue;
            }
            apts_gpu = vframe_gpu_seek->pts;
            //已经到了用户选择的帧了。
            vpts_seek_gpu = -1;
        }
    }
    while (true) {
        //确保把里面的视频帧都取出来
        result = avcodec_receive_frame(vc_gpu, vframe_gpu_seek);
        LOGE(" result %d ", result);
        if (result < 0) {
            break;
        }
    }
    //test
//    avio_flush(afc_gpu->pb);
    result = avformat_flush(afc_gpu);
    LOGE(" avformat_flush result %d ", result);
    justPlay_gpu();
    pthread_mutex_unlock(&mutex_pthread);
    return (void *) RESULT_SUCCESS;
}

int seekPos(double pos) {
    pthread_mutex_lock(&mutex_pthread);
    int result = -1;
    //用户点击进度条跳转的地方
    vpts_gpu = (int64_t) (pos * 1000 * afc_gpu->streams[video_index_gpu]->duration *
                          av_q2d(afc_gpu->streams[video_index_gpu]->time_base));

    vpts_seek_gpu = afc_gpu->streams[video_index_gpu]->duration * pos;
    LOGE("*************      vpts_seek_gpu %lld , after %d  ", vpts_seek_gpu,getConvertPts(vpts_seek_gpu ,afc_gpu->streams[video_index_gpu]->time_base ));
    avio_flush(afc_gpu->pb);
    result = avformat_flush(afc_gpu);

    if (result < 0) {
        LOGE(" avformat_flush result %d ", result);
        return RESULT_FAILD;
    }

    clearAllQue();
    //这里会直接会跳转到之前的关键帧
    result = av_seek_frame(afc_gpu , video_index_gpu , vpts_seek_gpu , AVSEEK_FLAG_BACKWARD);

    if (result < 0) {
        LOGE(" av_seek_frame faild result %d ", result);
        return RESULT_FAILD;
    }
    pthread_create(&jumpToSeekPosThread, NULL, jumpToSeekFrame, NULL);
    pthread_mutex_unlock(&mutex_pthread);
    return RESULT_SUCCESS;
}

//获取当前的播放位置，最大值是100，0~100
int getPlayPosition() {
    LOGE(" getPlayPosition vpts_gpu %lld  videoDuration_gpu %lld , poition %d ", vpts_gpu,
         videoDuration_gpu, ((int) ((double) vpts_gpu / (double) videoDuration_gpu *
                                    100)));

    return videoDuration_gpu <= 0 ? 0 : (int) ((double) vpts_gpu / (double) videoDuration_gpu *
                                               100);
}

int destroy_FFmpeg() {

    if (aframe_gpu != NULL) {
        LOGE(" av_frame_free aframe_gpu ");
        av_frame_free(&aframe_gpu);
    }
    if (vframe_gpu != NULL) {
        LOGE(" av_frame_free vframe_gpu ");
        av_frame_free(&vframe_gpu);
    }
    if (vframe_gpu_seek != NULL) {
        LOGE(" av_frame_free vframe_gpu ");
        av_frame_free(&vframe_gpu_seek);
    }
    if (vc_gpu != NULL) {
        LOGE(" avcodec_close vc_gpu ");
        avcodec_close(vc_gpu);
    }
    if (ac_gpu != NULL) {
        LOGE(" avcodec_close ac_gpu ");
        avcodec_close(ac_gpu);
    }
    if (afc_gpu != NULL) {
        LOGE(" avformat_free_context afc_gpu ");

//        avformat_free_context(afc_gpu); 这个函数主要用来混合成一个文件时用的，将音频视频混合一个MP4文件，最后释放
        avformat_close_input(&afc_gpu);

    }
    if (swc_gpu != NULL) {
        LOGE(" swr_close swc_gpu ");
        swr_free(&swc_gpu);
//        swr_close(swc_gpu); 这个不是释放，调用了这个还可以再直接调用swr_init();
    }

    return 1;
}

int destroy_Audio() {
    apts_gpu = 0;
    if (iplayer_gpu && (*iplayer_gpu)) {
        (*iplayer_gpu)->SetPlayState(iplayer_gpu, SL_PLAYSTATE_STOPPED);
    }
    if (pcmQue_gpu != NULL) {
        (*pcmQue_gpu)->Clear(pcmQue_gpu);
    }
    if (player_gpu != NULL) {
        (*player_gpu)->Destroy(player_gpu);
        player_gpu = NULL;
        iplayer_gpu = NULL;
        pcmQue_gpu = NULL;
        LOGE("audio player_gpu destory ! ");
    }

    if (mix_gpu != NULL) {
        (*mix_gpu)->Destroy(mix_gpu);
        mix_gpu = NULL;
        LOGE("audio mix_gpu destory ! ");
    }
    if (engineOpenSL_gpu != NULL) {
        (*engineOpenSL_gpu)->Destroy(engineOpenSL_gpu);
        engineOpenSL_gpu = NULL;
        eng_gpu = NULL;
        LOGE("audio engineOpenSL_gpu destory ! ");
    }
    delete play_audio_buffer;
    delete play_audio_temp;
    return 1;
}

int destroyShader() {
//    EGLDisplay display;
    initOpenglFlag = false;
    glDisableVertexAttribArray(apos);
    glDisableVertexAttribArray(atex);
    glDetachShader(vsh, program);
    glDetachShader(fsh, program);
    glDeleteShader(vsh);
    glDeleteShader(fsh);
    glDeleteProgram(program);
    glDeleteTextures(3, texts);
    eglDestroyContext(display, context);
    eglDestroySurface(display, winsurface);
    eglTerminate(display);
    if (nwin != NULL) {
        ANativeWindow_release(nwin);
    }
    LOGE(" destroyShader ");
    return RESULT_SUCCESS;
}

int destroyOther() {
    pauseFlag = false;
    return RESULT_SUCCESS;
}

int destroy_gpu() {
    destroyOther();
    stopAllThread();
    destroy_FFmpeg();
    destroy_Audio();
    destroyShader();
    return 1;
}

