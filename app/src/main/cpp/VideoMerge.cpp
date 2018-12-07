//
// Created by Administrator on 2018/12/7/007.
//


#include "VideoMerge.h"

VideoMerge::VideoMerge(vector<char *> inputPath, const char *output) {
    av_register_all();
    int ret;
    int size = inputPath.size();
    afcInputs = (AVFormatContext **) av_malloc(size * sizeof(AVFormatContext *)); //新建几个指针变量
    for (int i = 0; i < size; ++i) {
        ret = initInput(&afcInputs[i], inputPath[i]);
        if (ret < 0) {
            LOGE(" init input faild !");
            return;
        }
    }
    ret = buildOutput(output);
    if (ret < 0) {
        LOGE(" build output faild !");
        return;
    }
    ret = buildFilter();
    if (ret < 0) {
        LOGE(" buildFilter faild !");
        return;
    }
}

int VideoMerge::start() {

    return 1;
}

int VideoMerge::initInput(AVFormatContext **afcInput, char *inputPath) {
    int result;
    *afcInput = NULL;
    open_input_file(inputPath, afcInput);
    MyInputContext *myInputContext = new MyInputContext();
    myInputContext->afcInput = *afcInput;
    for (int i = 0; i < (*afcInput)->nb_streams; i++) {
        AVStream *stream;
        stream = (*afcInput)->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            /* Open decoder */
            myInputContext->videoStreamIndex = i;
            AVCodec *vDcode = avcodec_find_decoder(stream->codecpar->codec_id);
            myInputContext->vDec = avcodec_alloc_context3(vDcode);
            if (myInputContext->vDec == NULL) {
                LOGE(" avcodec_alloc_context3 faild ！");
                return result;
            }
            result = avcodec_open2(myInputContext->vDec,
                                   avcodec_find_decoder(myInputContext->vDec->codec_id), NULL);
            if (result < 0) {
                LOGE(" avcodec_open2 faild ！");
                return result;
            }
        } else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            myInputContext->audioStreamIndex = i;
            AVCodec *vDcode = avcodec_find_decoder(stream->codecpar->codec_id);
            myInputContext->aDec = avcodec_alloc_context3(vDcode);
            if (myInputContext->aDec == NULL) {
                LOGE(" avcodec_alloc_context3 faild ！");
                return result;
            }
            result = avcodec_open2(myInputContext->aDec,
                                   avcodec_find_decoder(myInputContext->aDec->codec_id), NULL);
            if (result < 0) {
                LOGE(" avcodec_open2 faild ！");
                return result;
            }
        }
    }
    myInputContexts.push_back(myInputContext);
    LOGE(" init input success !");
    return 1;
}

int VideoMerge::buildOutput(const char *output) {
    int result = 0;
    outWidth = 640;
    outHeight = 360;
    afcOutput = NULL;
    result = initOutput(output, &afcOutput);
    if (result < 0) {
        LOGE(" init output faild ! ");
        return -1;
    }
//    AVOutputFormat *afot = afcOutput->oformat;
    LOGE(" inputcontext %d ", myInputContexts.size());
    MyInputContext *myInputContext = myInputContexts.at(0);
    AVCodecParameters *vparams = myInputContext->afcInput->streams[myInputContext->videoStreamIndex]->codecpar;
    AVCodecParameters *outputVParams = avcodec_parameters_alloc();
    avcodec_parameters_copy(outputVParams, vparams);
    outputVParams->width = outWidth;
    outputVParams->height = outHeight;
    result = addOutputVideoStream(afcOutput, &vEnconde, *outputVParams);
    avcodec_parameters_free(&outputVParams);
    if (result < 0) {
        LOGE(" addOutputVideoStream faild ! ");
        return -1;
    }

    AVCodecParameters *aparams = myInputContext->afcInput->streams[myInputContext->audioStreamIndex]->codecpar;
    AVCodecParameters *outputAParams = avcodec_parameters_alloc();
    avcodec_parameters_copy(outputAParams, aparams);
    result = addOutputAudioStream(afcOutput, &aEnconde, *outputAParams);
    avcodec_parameters_free(&outputAParams);
    if (result < 0) {
        LOGE(" addOutputAudioStream faild ! ");
        return -1;
    }
    result = writeOutoutHeader(afcOutput, output);
    if (result < 0) {
        LOGE(" writeOutoutHeader faild ! ");
        return -1;
    }
    return 1;
}

int VideoMerge::buildFilter() {

    int size = inputPaths.size();

    AVFilter *bufferSink = avfilter_get_by_name("buffersink");
    AVFilterContext *buffersinkCtx = NULL;
    AVFilterInOut *inputs = avfilter_inout_alloc();

    AVFilter **bufferSrcs = (AVFilter **) av_malloc(sizeof(AVFilter *) * size);
    AVFilterContext **buffersrcCtx = (AVFilterContext **) av_malloc(sizeof(AVFilterContext *) * size);
    AVFilterInOut **outputs = (AVFilterInOut **) av_malloc(size * sizeof(AVFilterInOut *));

    AVFilterGraph *filterGraph = avfilter_graph_alloc();

    for (int i = 0; i < size; i++) {
        bufferSrcs[i] = NULL;
        buffersrcCtx[i] = NULL;
        outputs[i] = avfilter_inout_alloc();
    }
    if (!outputs || !inputs || !filterGraph) {
        LOGE(" avfilter_inout_alloc FAILD !");
        goto end;
    }


    return 1;

end :
    avfilter_graph_free(&filterGraph);
    avfilter_inout_free(outputs);
    avfilter_inout_free(&inputs);
    av_free(bufferSrcs);
    av_free(buffersrcCtx);
    av_free(outputs);
    return -1;
}


VideoMerge::~VideoMerge() {

}

