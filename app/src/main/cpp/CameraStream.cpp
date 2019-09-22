#include <malloc.h>
#include <CallJava.h>
#include "CameraStream.h"
#include <my_log.h>


/**
 * 将视频，和声音录制了，然后再编码处理。
 * 这里需要采取丢帧方式处理了。
 * 当视频 > 音频 + 50 的时候视频需要开始丢帧
 * 当音频 > 视频 + 50 的时候音频需要丢帧
 *
 * 因为视频部分进来后会旋转90度所以注意宽高的对调
 */
CameraStream::CameraStream() {
  afc_output = NULL;
  sws = NULL;
  audioStream = NULL;
  videoStream = NULL;
  fifo = NULL;
  outputWidth = 640;
  outputHeight = 480;
  apts = 0;
  vpts = 0;
  aCount = 0;
  vCount = 0;
  dropACount = 0;
  dropVCount = 0;
  outSampleRate = 11025;
  outSampleForamt = AV_SAMPLE_FMT_S16;
  initSuccess = false;
  yuvRotate = (char *) malloc(outputWidth * outputHeight * 3 / 2);
}

int CameraStream::init(const char *url, int width, int height, int pcmsize, CallJava *cj) {
  callJava = cj;
  this->inputWidth = width;
  this->inputHeight = height;

  int ret = avformat_network_init();
  if (ret < 0) {
    callJava->callStr(" avformat_network_init faild !");
    return -1;
  }
  ret = initOutput(url, "flv", &afc_output);
  if (ret < 0) {
    callJava->callStr(" initOutput faild !");
    return -1;
  }

  //video
  AVCodecParameters *vparams = avcodec_parameters_alloc();
  vparams->format = AV_PIX_FMT_YUV420P;
  vparams->width = outputWidth;
  vparams->height = outputHeight;
  ret = addOutputVideoStream(afc_output, &vCtxE, *vparams);
  avcodec_parameters_free(&vparams);
  if (ret < 0) {
    callJava->callStr(" addOutputVideoStream faild !");
    return -1;
  }
  videoStream = afc_output->streams[ret];


  //audio
  AVCodecParameters *aparams = avcodec_parameters_alloc();
  aparams->format = outSampleForamt;
  aparams->sample_rate = outSampleRate;
  aparams->channels = 1;
  aparams->channel_layout = AV_CH_LAYOUT_MONO;
  ret = addOutputAudioStream(afc_output, &aCtxE, *aparams);
  avcodec_parameters_free(&aparams);
  if (ret < 0) {
    callJava->callStr(" addOutputAudioStream faild !");
    return -1;
  }
  audioStream = afc_output->streams[ret];
  nbSample = aCtxE->frame_size;
  allocAudioFifo(outSampleForamt, 1, nbSample * 4);

  ret = writeOutoutHeader(afc_output, url);
  if (ret < 0) {
    callJava->callStr(" writeOutoutHeader faild !");
    return -1;
  }

  this->start();
  setPause();

  if (width != outputWidth || height != outputHeight) {
    LOGE(" WIDTH %d  ， height %d  , pix %d ", width, height, vCtxE->pix_fmt);
    if (initSwsContext(width, height, vCtxE->pix_fmt) < 0) {
      callJava->callStr(" initSwsContext faild !");
      return -1;
    }
  }

  framePic = av_frame_alloc();
  framePic->width = width;
  framePic->height = height;
  framePic->format = AV_PIX_FMT_YUV420P;



  frameOutA = av_frame_alloc();
  frameOutA->sample_rate = outSampleRate;
  frameOutA->channels = 1;
  frameOutA->channel_layout = AV_CH_LAYOUT_MONO;
  frameOutA->format = outSampleForamt;
  frameOutA->nb_samples = aCtxE->frame_size;


  aCalDuration = AV_TIME_BASE / outSampleRate;
  vCalDuration = AV_TIME_BASE / outFrameRate;
  initSuccess = true;

  return 1;
}

void CameraStream::destroyAudioFifo() {
  if (fifo != NULL) {
    av_audio_fifo_free(fifo);
    fifo = NULL;
  }
}

void CameraStream::allocAudioFifo(AVSampleFormat sample_fmt, int channels, int nb_samples) {
  if (fifo == NULL) {
    fifo = av_audio_fifo_alloc(sample_fmt, channels, nb_samples);
  }
};


int CameraStream::initSwsContext(int inWidth, int inHeight, int inpixFmt) {
  LOGE("XHC OUTWIDTH %d , outheight %d ", outputWidth, outputHeight);
  sws = sws_getContext(inWidth, inHeight, (AVPixelFormat) inpixFmt, outputWidth, outputHeight,
                       AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
  if (sws == NULL) {
    return -1;
  }
  return 1;
}

void CameraStream::destroySwsContext() {
  if (sws != NULL) {
    sws_freeContext(sws);
    sws = NULL;
  }
}


int CameraStream::startRecord() {
  setPlay();
  return 1;
}

int CameraStream::pauseRecord() {
  setPause();
  return 1;
}


void CameraStream::pushAudioStream(jbyte *pcm, int size) {
  if (pause || isExit || !initSuccess) {
    return;
  }
  int queSize = (int) audioPktQue.size() - (int) videoPktQue.size();
  if (queSize > 40) {
    dropACount++;
    if (dropACount % 2 == 0) {
      LOGE(" DROP aaaaaaa frmae ");
      return; //丢弃
    }
  } else if (queSize > 20) {
    dropACount++;
    if (dropACount % 3 == 0) {
      LOGE(" DROP aaaaaaa frmae ");
      return; //丢弃
    }
  }

  av_audio_fifo_write(fifo, (void **) &pcm, size / av_get_bytes_per_sample(outSampleForamt));
  if (av_audio_fifo_size(fifo) < nbSample) {
    return;
  }
  av_audio_fifo_read(fifo, (void **) &pcm, nbSample);
  frameOutA->data[0] = (uint8_t *) pcm;
  frameOutA->linesize[0] = nbSample * av_get_bytes_per_sample(outSampleForamt);
  frameOutA->pts = aCount * aCalDuration;
  aCount += size / av_get_bytes_per_sample(outSampleForamt);
  AVPacket *pkt = encodeFrame(frameOutA, aCtxE);
  if (pkt != NULL) {
    pkt->stream_index = audioStream->index;
    pkt->dts = pkt->pts;
    audioPktQue.push(pkt);
  }

}

void CameraStream::pushVideoStream(jbyte *yuv) {
  if (pause || isExit || !initSuccess) {
    return;
  }

  int queSize = (int) videoPktQue.size() - (int) audioPktQue.size();
  LOGE(" QUESIZE %d ", queSize);
  if (queSize > 40) {
    dropVCount++;
    if (dropVCount % 2 == 0) {
      LOGE(" DROP vvvvvvvvv frmae ");
      return;
    }
  } else if (queSize > 20) {
    dropVCount++;
    if (dropVCount % 3 == 0) {
      LOGE(" DROP vvvvvvvvv frmae ");
      return;
    }
  }
  int ySize = inputWidth * inputHeight;
//    rotateRectAnticlockwiseDegree90((char *) yuv, 0, inputWidth, inputHeight, yuvRotate, 0);
//    rotateRectAnticlockwiseDegree90((char *) yuv, ySize, inputWidth / 2, inputHeight / 2, yuvRotate,ySize);
//    rotateRectAnticlockwiseDegree90((char *) yuv, ySize * 5 / 4, inputWidth / 2, inputHeight / 2,yuvRotate, ySize * 5 / 4);
//    rotateRectAnticlockwiseDegree90((char *) yuv, 0, inputWidth, inputHeight, yuvRotate, 0);
//    rotateRectAnticlockwiseDegree90((char *) yuv, ySize, inputWidth / 2, inputHeight / 2, yuvRotate, ySize);
//    rotateRectAnticlockwiseDegree90((char *) yuv, ySize * 5 / 4, inputWidth / 2, inputHeight / 2, yuvRotate, ySize * 5 / 4);
  framePic->data[0] = (uint8_t *) (yuv);
  framePic->data[1] = (uint8_t *) (yuv + inputWidth * inputHeight * 5 / 4);
  framePic->data[2] = (uint8_t *) (yuv + inputWidth * inputHeight);

  framePic->linesize[0] = inputWidth;
  framePic->linesize[1] = inputWidth / 2;
  framePic->linesize[2] = inputWidth / 2;

//  if (vCount == 10) {
//    LOGE("XHC WIDTH %d , height %d , linseize [0] %d ", inputWidth, inputHeight , framePic->linesize[0]);
//    FILE *file = fopen("sdcard/FFmpeg/test_o.yuv", "wb+");
//    fwrite(framePic->data[0], 1, inputHeight * inputWidth , file);
//    fwrite(framePic->data[1], 1, inputHeight * inputWidth / 4 , file);
//    fwrite(framePic->data[2], 1, inputHeight * inputWidth / 4, file);
//    fclose(file);
//  }

  //修改分辨率统一输出大小
  AVPacket *pkt = NULL;
  if (sws != NULL) {
    frameOutV = av_frame_alloc();
    frameOutV->width = outputWidth;
    frameOutV->height = outputHeight;
    frameOutV->format = AV_PIX_FMT_YUV420P;
    av_frame_get_buffer(frameOutV, 0);

    sws_scale(sws, (const uint8_t *const *) framePic->data, framePic->linesize,
              0, framePic->height, frameOutV->data, frameOutV->linesize);

    frameOutV->pts = vCount * vCalDuration;

//    if (vCount == 10) {
//      LOGE(" OUTPUT WIDTH %d , height %d  , linesize %d" , outputWidth , outputHeight , frameOutV->linesize[0]);
//      FILE *file = fopen("sdcard/FFmpeg/testcamera.yuv", "wb+");
//      fwrite(frameOutV->data[0], 1, outputWidth * outputHeight, file);
//      fwrite(frameOutV->data[1], 1, outputWidth * outputHeight / 4, file);
//      fwrite(frameOutV->data[2], 1, outputWidth * outputHeight / 4, file);
//      fclose(file);
//    }

    pkt = encodeFrame(frameOutV, vCtxE);
    av_frame_free(&frameOutV);
    frameOutV = NULL ;

  } else {
    framePic->linesize[0] = outputWidth;
    framePic->linesize[1] = outputWidth / 2;
    framePic->linesize[2] = outputWidth / 2;
    framePic->pts = vCount * vCalDuration;
    pkt = encodeFrame(framePic, vCtxE);
  }
  vCount++;
  if (pkt != NULL) {
    pkt->dts = pkt->pts;
    pkt->stream_index = videoStream->index;
    videoPktQue.push(pkt);
  }
}


void CameraStream::rotateRectAnticlockwiseDegree90(char *src, int srcOffset, int width, int height,
                                                   char *dst, int dstOffset) {
  int i, j;
  int index = dstOffset;
  for (i = 0; i < width; i++) {
    for (j = height - 1; j >= 0; j--) {
      dst[index] = src[srcOffset + j * width + i];
      index++;
    }
  }
}

//新开一个线程来混合
void CameraStream::run() {
  while (!isExit) {
    if (pause) {
      threadSleep(3);
      continue;
    }
    if (videoPktQue.size() <= 0 || audioPktQue.size() <= 0
        || audioStream == NULL || videoStream == NULL) {
      threadSleep(2);
      continue;
    }
    LOGE(" VIDEO SIZE %d ,  AUDIO size  %d ", videoPktQue.size(), audioPktQue.size());
    AVPacket *aPkt = audioPktQue.front();
    AVPacket *vPkt = videoPktQue.front();
    if (av_compare_ts(apts, audioStream->time_base, vpts, videoStream->time_base) < 0) {
      av_packet_rescale_ts(aPkt, timeBaseFFmpeg, audioStream->time_base);
//            LOGE(" WIRTE AAAAAA %lld ,  %lld  ", aPkt->dts , aPkt->pts);
      apts = aPkt->pts;

      av_interleaved_write_frame(afc_output, aPkt);
      audioPktQue.pop();
    } else {
      av_packet_rescale_ts(vPkt, timeBaseFFmpeg, videoStream->time_base);
//            LOGE(" WIRTE vvvvvvvvvvv %lld ,  %lld  ", vPkt->dts , vPkt->pts);
      vpts = vPkt->pts;

      av_interleaved_write_frame(afc_output, vPkt);
      videoPktQue.pop();
    }
  }
}


CameraStream::~CameraStream() {
  this->stop();
  this->join();
  while (!videoPktQue.empty()) {
    AVPacket *pkt = videoPktQue.front();
    if (pkt != NULL) {
      av_packet_free(&pkt);
    }
    videoPktQue.pop();
  }
  while (!audioPktQue.empty()) {
    AVPacket *pkt = audioPktQue.front();
    if (pkt != NULL) {
      av_packet_free(&pkt);
    }
    audioPktQue.pop();
  }
  if (afc_output != NULL && afc_output->oformat != NULL && afc_output->pb != NULL) {
    av_write_trailer(afc_output);
  }

  destroySwsContext();
  destroyAudioFifo();
  if (aCtxE != NULL) {
    avcodec_free_context(&aCtxE);
  }
  if (vCtxE != NULL) {
    avcodec_free_context(&vCtxE);
  }
  if (afc_output != NULL) {
    avformat_free_context(afc_output);
  }
  if (framePic != NULL) {
    av_frame_free(&framePic);
  }
  if (frameOutV != NULL) {
    av_frame_free(&frameOutV);
  }
  if (frameOutA != NULL) {
    av_frame_free(&frameOutA);
  }

  LOGE(" DESTROY CAMERA !! ");
}