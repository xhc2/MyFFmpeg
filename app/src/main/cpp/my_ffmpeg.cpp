//
// Created by Administrator on 2018/4/21/021.
//

//adb logcat | ndk-stack -sym obj/local/armeabi 查看底层错误栈
//NDK 使用 addr2line 调试工具
#include <jni.h>
#include <my_log.h>
#include <Mp4Player.h>
#include <gpu_video_sl_audio.h>
#include <CallJava.h>
#include <PublishStream.h>
#include <CameraStream.h>
#include <decode_test.h>
#include <SRSLibRtmp.h>
#include <aacparse.h>
#include <OpenGlTest.h>
#include <VideoJoint.h>
#include <VideoRunBack.h>
#include <CurrentTimeBitmap.h>
#include <my_open_sl_test.h>
#include <NewAudioPlayer.h>
#include <GifMake.h>
#include <VideoMerge.h>
#include <AudioMix.h>
#include <VideoDub.h>
#include "FlvParse.h"
#include "h264Parse.h"
#include "VideoClip.h"
#include "VideoFilter.h"

Mp4Player *mp4Player = NULL;
PublishStream *ps = NULL;
CallJava *cj = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initMp4Play(JNIEnv *env, jclass type, jstring path_,
                                                       jobject glSurfaceView) {


    if (mp4Player == NULL) {
        if (cj == NULL) {
            cj = new CallJava(env, type);
        }

        const char *path = env->GetStringUTFChars(path_, 0);
        ANativeWindow *win = ANativeWindow_fromSurface(env, glSurfaceView);
        mp4Player = new Mp4Player(path, win, cj);
        env->ReleaseStringUTFChars(path_, path);
    }
    return 1;

}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getVideoWidth(JNIEnv *env, jclass type) {

    // TODO
    if (mp4Player != NULL) {
        return mp4Player->getVideoWidth();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getVideoHeight(JNIEnv *env, jclass type) {

    // TODO
    if (mp4Player != NULL) {
        return mp4Player->getVideoHeight();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyMp4Play(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        delete mp4Player;

    }
    mp4Player = NULL;
    if (cj != NULL) {
        delete cj;

    }
    cj = NULL;
    return 1;

}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_mp4Pause(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        mp4Player->pauseVA();
        return 1;
    }

    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_mp4Play(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        mp4Player->playVA();
        return 1;
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_changeSpeed(JNIEnv *env, jclass type, jfloat speed) {
    // TODO
    if (mp4Player != NULL) {
        mp4Player->changeSpeed(speed);
        return 1;
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getProgress(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        return mp4Player->getProgress();
    }
    return 0;
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getDuration(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        return mp4Player->getDuration();
    }
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_seekStart(JNIEnv *env, jclass type) {
    if (mp4Player != NULL) {
        mp4Player->seekStart();
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_seek(JNIEnv *env, jclass type, jfloat progress) {
    if (mp4Player != NULL) {
        mp4Player->seek(progress);
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpInit(JNIEnv *env, jclass type, jstring path_,
                                                    jstring inpath_) {

    const char *path = env->GetStringUTFChars(path_, 0);
    const char *inPath = env->GetStringUTFChars(inpath_, 0);
    if (ps == NULL) {
        if (cj == NULL) {
            cj = new CallJava(env, type);
        }
        ps = new PublishStream(path, inPath, cj);
    }
    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(inpath_, inPath);

    return 1;

}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpClose(JNIEnv *env, jclass type) {

    // TODO
    if (ps != NULL) {
        delete ps;

    }
    ps = NULL;
    if (cj != NULL) {
        delete cj;

    }
    cj = NULL;
    return 1;
}
CameraStream *cs = NULL;
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpCameraInit(JNIEnv *env, jclass type,
                                                          jstring outPath_,
                                                          jint width, jint height, jint pcmSize) {
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    if (cs == NULL) {
        if (cj == NULL) {
            cj = new CallJava(env, type);
        }
        cs = new CameraStream(outPath, width, height, pcmSize, cj);
    }

    env->ReleaseStringUTFChars(outPath_, outPath);
    return 1;
}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpCameraStream(JNIEnv *env, jclass type,
                                                            jbyteArray bytes_) {
    jbyte *bytes = env->GetByteArrayElements(bytes_, NULL);

    if (cs != NULL) {
        cs->pushVideoStream(bytes);
    }

    env->ReleaseByteArrayElements(bytes_, bytes, 0);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpDestroy(JNIEnv *env, jclass type) {

    if (cs != NULL) {
        delete cs;
        cs = NULL;
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_rtmpAudioStream(JNIEnv *env, jclass type,
                                                           jbyteArray bytes_, jint size) {
    jbyte *bytes = env->GetByteArrayElements(bytes_, NULL);
    if (cs != NULL) {
        cs->pushAudioStream(bytes, size);
    }
    env->ReleaseByteArrayElements(bytes_, bytes, 0);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startRecord(JNIEnv *env, jclass type) {

    if (cs != NULL) {
        return cs->startRecord();
    }

    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_pauseRecord(JNIEnv *env, jclass type) {

    if (cs != NULL) {
        return cs->pauseRecord();
    }

    return 1;
}

SRSLibRtmp *srs;
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_srsTest(JNIEnv *env, jclass type, jstring outPath_) {
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    srs = new SRSLibRtmp();
    srs->publishH264(outPath);
    env->ReleaseStringUTFChars(outPath_, outPath);
    // TODO
    return 1;
}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_srsDestroy(JNIEnv *env, jclass type) {
    if (srs != NULL) {
        srs->rtmpDestroy();
    }
    return 1;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_flvParse(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);


    FlvParse *fp = new FlvParse(path);
    const char *result = fp->start();

    env->ReleaseStringUTFChars(path_, path);
    return env->NewStringUTF(result);
}

h264Parse *hp = NULL;
extern "C"
JNIEXPORT jstring JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_h264Parse(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    // TODO
    if (hp == NULL) {
        hp = new h264Parse(path);
    }
    hp->start();
    env->ReleaseStringUTFChars(path_, path);
    return env->NewStringUTF("h264解析完毕");
}






extern "C"
JNIEXPORT jbyteArray JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getNextNalu(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    if (hp == NULL) {
        hp = new h264Parse(path);
    }
    NALU *temp = hp->getNextNalu();

    if (temp != NULL) {
        jbyteArray array = env->NewByteArray(temp->size + temp->startCodeSize);
        env->SetByteArrayRegion(array, 0, temp->size + temp->startCodeSize, (jbyte *) temp->data);
        free(temp);
        env->ReleaseStringUTFChars(path_, path);
        return array;
    }
    env->ReleaseStringUTFChars(path_, path);
    return NULL;
}

AACParse *aacParse = NULL;

extern "C"
JNIEXPORT jstring JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_aacParse__Ljava_lang_String_2(JNIEnv *env, jclass type,
                                                                         jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    if (aacParse == NULL) {
        aacParse = new AACParse(path);
    }
    aacParse->parseStart();
    env->ReleaseStringUTFChars(path_, path);

    return env->NewStringUTF("aac解析完毕");
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getAACFrame(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    if (aacParse == NULL) {
        aacParse = new AACParse(path);
    }
    AACFrame *temp = aacParse->getAACFrame(false);
    if (temp != NULL) {
        jbyteArray array = env->NewByteArray(temp->size);
        env->SetByteArrayRegion(array, 0, temp->size, (jbyte *) temp->data);
        free(temp);
        env->ReleaseStringUTFChars(path_, path);
        return array;
    }
    env->ReleaseStringUTFChars(path_, path);
    return NULL;
}

VideoClip *vc;

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startClip(JNIEnv *env, jclass type, jstring path_,
                                                     jstring output_,
                                                     jint start, jint end) {
    const char *path = env->GetStringUTFChars(path_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);
    LOGE(" OUTPUT %s ", output);
    if (vc == NULL) {
        vc = new VideoClip(path, output, start, end);
    }
    vc->startClip();
    env->ReleaseStringUTFChars(path_, path);
    env->ReleaseStringUTFChars(output_, output);
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getClipProgress(JNIEnv *env, jclass type) {
    if (vc != NULL) {
        return vc->getClipProgress();
    }
    return -1;
}

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyClip(JNIEnv *env, jclass type) {
    if (vc != NULL) {
        delete vc;
    }
    vc = NULL;
}


OpenGlTest *ot;
extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_openGlTest(JNIEnv *env, jclass type, jstring path_,
                                                      jobject glSurfaceView) {
    const char *path = env->GetStringUTFChars(path_, 0);
    ANativeWindow *win = ANativeWindow_fromSurface(env, glSurfaceView);
    ot = new OpenGlTest(path, win);
    ot->start();
    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_openDestroy(JNIEnv *env, jclass type) {

    delete ot;
    // TODO

}

VideoJoint *vj;

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startJoint(JNIEnv *env, jclass type, jobjectArray paths,
                                                      jstring output_, jint outWidth,
                                                      jint outHeight) {
    const char *output = env->GetStringUTFChars(output_, 0);
    int size = env->GetArrayLength(paths);

    if (size > 10) {
        //抛出异常
        jclass clazz;
        clazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(clazz, " size should be < 10");
        return;
    }

    LOGE(" INPUT SIZE %d ", size);
//    char *inputPath[size];
    std::vector<char *> inputPaths;
    for (int i = 0; i < size; i++) {
        jstring obja = (jstring) env->GetObjectArrayElement(paths, i);
        const char *chars = env->GetStringUTFChars(obja, NULL);
        int charLen = strlen(chars);
        charLen++;
        char *temp = (char *) malloc(charLen);
        strcpy(temp, chars);
        inputPaths.push_back(temp);
        env->ReleaseStringUTFChars(obja, chars);
    }

    if (vj == NULL) {
        vj = new VideoJoint(inputPaths, output, outWidth, outHeight);
    }
    vj->startJoint();
    env->ReleaseStringUTFChars(output_, output);
}
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getJointProgress(JNIEnv *env, jclass type) {

    // TODO
    if (vj != NULL) {
        return vj->getProgress();
    }
    return -1;
}

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyJoint(JNIEnv *env, jclass type) {

    if (vj != NULL) {
        delete vj;

    }
    vj = NULL;
}

VideoRunBack *vb;

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startBackRun(JNIEnv *env, jclass type,
                                                        jstring inputPath_, jstring output_) {
    const char *inputPath = env->GetStringUTFChars(inputPath_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);
    if (vb == NULL) {
        vb = new VideoRunBack(inputPath, output);
    }
    vb->startBackParse();
    env->ReleaseStringUTFChars(inputPath_, inputPath);
    env->ReleaseStringUTFChars(output_, output);
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getBackRunProgress(JNIEnv *env, jclass type) {

    // TODO
    if (vb != NULL) {
        return vb->getProgress();
    }
    return -1;
}

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyBackRun(JNIEnv *env, jclass type) {

    if (vb != NULL) {
        delete vb;
    }
    vb = NULL;
}

CurrentTimeBitmap *cb = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyCurrentBitmap(JNIEnv *env, jclass type) {

    if (cb != NULL) {
        delete cb;
    }
    cb = NULL;
}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initCurrentBitmp(JNIEnv *env, jclass type, jstring path_,
                                                            jint outWidth, jint outHeight) {
    const char *path = env->GetStringUTFChars(path_, 0);

    if (cb == NULL) {
        cb = new CurrentTimeBitmap(path, outWidth, outHeight);
    }
    env->ReleaseStringUTFChars(path_, path);
    return 1;
}


extern "C"
JNIEXPORT jfloat JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getCurrentBitmp(JNIEnv *env, jclass type, jfloat time,
                                                           jbyteArray result_) {
    jbyte *result = env->GetByteArrayElements(result_, NULL);

    cb->getCurrentBitmapKeyFrame(time, (uint8_t *) result);

    env->ReleaseByteArrayElements(result_, result, 0);
    return time;
}


//test
NewAudioPlayer *a = NULL;
// AudioPlayer  *a = NULL;
FILE *file;
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_test(JNIEnv *env, jclass type) {
//    createEngine();
//    createBufferQueueAudioPlayer();
//    startPlayTest();
    file = fopen("sdcard/FFmpeg/test_2c_441_16.pcm", "r");
    fseek(file, -2048 * 500, SEEK_END);
    a = new NewAudioPlayer(44100, 2);
//    a = new AudioPlayer(44100 , 2);
    a->changeSpeed(1.0f);
    a->start();
    while (true) {
        char *temp = (char *) malloc(2048);
        int len = fread(temp, 1, 2048, file);
        if (len != 2048) {
            a->update(NULL);
            break;
        }
        MyData *myData = new MyData();
        myData->data = temp;
        myData->size = 2048;
        myData->isAudio = true;
        a->update(myData);
    }

    return 1;

}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_test2(JNIEnv *env, jclass type) {
    fclose(file);
    delete a;
    LOGE(" DELETE AUDIO PLAYER ");
    return 1;
}

VideoFilter *bwm = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initVideoFilter(JNIEnv *env, jclass type,
                                                           jstring videoPath_, jstring outputPath_,
                                                           jstring filterDescr_,
                                                           jintArray params_) {
    const char *videoPath = env->GetStringUTFChars(videoPath_, 0);
    const char *outputPath = env->GetStringUTFChars(outputPath_, 0);
    const char *filterDescr = env->GetStringUTFChars(filterDescr_, 0);
    jint *params = env->GetIntArrayElements(params_, NULL);
    int size = env->GetArrayLength(params_);
    if (bwm == NULL) {
        bwm = new VideoFilter(videoPath, outputPath, filterDescr, params, size);
    }
    env->ReleaseStringUTFChars(videoPath_, videoPath);
    env->ReleaseStringUTFChars(outputPath_, outputPath);
    env->ReleaseStringUTFChars(filterDescr_, filterDescr);
    env->ReleaseIntArrayElements(params_, params, 0);

}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_videoFilterStart(JNIEnv *env, jclass type) {

    // TODO
    if (bwm != NULL) {
        bwm->startWaterMark();
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getVideoFilterProgress(JNIEnv *env, jclass type) {

    // TODO
    if (bwm != NULL) {
        return bwm->getProgress();
    }
    return -1;

}extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_videoFilterDestroy(JNIEnv *env, jclass type) {

    // TODO
    if (bwm != NULL) {
        delete bwm;
    }
    bwm = NULL;
    return 0;
}


GifMake *gifMake;
extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initGif(JNIEnv *env, jclass type, jstring videoPath_,
                                                   jstring outPath_, jint start, jint end) {
    const char *videoPath = env->GetStringUTFChars(videoPath_, 0);
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    if (gifMake == NULL) {
        gifMake = new GifMake(videoPath, outPath, start, end);
    }
    env->ReleaseStringUTFChars(videoPath_, videoPath);
    env->ReleaseStringUTFChars(outPath_, outPath);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_getGifProgress(JNIEnv *env, jclass type) {

    // TODO
    if (gifMake != NULL) {
        return gifMake->getProgress();
    }
    return -1;
}


extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyGif(JNIEnv *env, jclass type) {

    // TODO
    if (gifMake != NULL) {
        delete gifMake;
    }
    gifMake = NULL;
    return -1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startGifParse(JNIEnv *env, jclass type) {

    // TODO
    if (gifMake != NULL) {
        gifMake->startParse();
    }
    return 1;
}


VideoMerge *vm;

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initVideoMerge(JNIEnv *env, jclass type,
                                                          jobjectArray inputPath,
                                                          jstring outputPath_) {
    const char *outputPath = env->GetStringUTFChars(outputPath_, 0);
    int size = env->GetArrayLength(inputPath);
    if (size > 4) {
        //抛出异常
        jclass clazz;
        clazz = env->FindClass("java/lang/Exception");
        env->ThrowNew(clazz, " size should be < 4");
        return -1;
    }

    LOGE(" INPUT SIZE %d ", size);
//    char *inputPath[size];
    std::vector<char *> inputPaths;
    for (int i = 0; i < size; i++) {
        jstring obja = (jstring) env->GetObjectArrayElement(inputPath, i);
        const char *chars = env->GetStringUTFChars(obja, NULL);
        int charLen = strlen(chars);
        charLen++;
        char *temp = (char *) malloc(charLen);
        strcpy(temp, chars);
        inputPaths.push_back(temp);

        env->ReleaseStringUTFChars(obja, chars);
    }
//    vm = new VideoMerge(inputPaths , outputPath);
    AudioMix am;
    am.startMix();
    env->ReleaseStringUTFChars(outputPath_, outputPath);

    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_startVideoMerge(JNIEnv *env, jclass type) {


    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_destroyVideoMerge(JNIEnv *env, jclass type) {

    return 1;
}

VideoDub *vd = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_initVideoDub(JNIEnv *env, jclass type,
                                                        jstring inputPath_, jstring outputPath_,
                                                        jobject glSurfaceView) {
    const char *inputPath = env->GetStringUTFChars(inputPath_, 0);
    const char *outputPath = env->GetStringUTFChars(outputPath_, 0);

    if (vd == NULL) {
        ANativeWindow *win = ANativeWindow_fromSurface(env, glSurfaceView);
        vd = new VideoDub(inputPath, outputPath, win);
    }
    vd->startDub();

    env->ReleaseStringUTFChars(inputPath_, inputPath);
    env->ReleaseStringUTFChars(outputPath_, outputPath);
    return 1;
}


//extern "C"
//JNIEXPORT jint JNICALL
//Java_module_video_jnc_myffmpeg_FFmpegUtils_startVideoDub(JNIEnv *env, jclass type, jboolean flag) {
//
//    if(vd != NULL){
//        vd->startDub();
//    }
//    return 1;
//}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_videoDubDestroy(JNIEnv *env, jclass type) {

    if (vd != NULL) {
        delete vd;
    }
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_setFlag(JNIEnv *env, jclass type, jboolean flag) {
    if (vd != NULL) {
        vd->setFlag(flag);
    }
    return 1;
}extern "C"
JNIEXPORT jint JNICALL
Java_module_video_jnc_myffmpeg_FFmpegUtils_videoDubAddVoice(JNIEnv *env, jclass type,
                                                            jbyteArray pcm_) {
    jbyte *pcm = env->GetByteArrayElements(pcm_, NULL);
    if (vd != NULL) {
        int size = env->GetArrayLength(pcm_);
        char *pcmArray = (char *) malloc(size);
        memcpy(pcmArray , pcm , size);
        vd->addVoice(pcmArray, size);
    }
    env->ReleaseByteArrayElements(pcm_, pcm, 0);
    return 1;
}