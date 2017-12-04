//
// Created by dugang on 2017/12/1.
//

#include <My_LOG.h>
#include <libavutil/pixfmt.h>
#include "stdio.h"
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

int swscale(const char *input_path, const char *output_path) {
    int ret = 0;
    int width = 480;
    int height = 272;
    const int dst_w = 1280, dst_h = 720;

    av_log_set_callback(custom_log);

    enum AVPixelFormat dst_pixfmt = AV_PIX_FMT_RGB24;
    int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

    FILE *iFile = fopen(input_path, "rb");
    if (iFile == NULL) {
        LOGE("open input faild !");
        return -1;
    }

    FILE *oFile = fopen(output_path, "wb+");
    if (oFile == NULL) {
        LOGE("open output faild !");
        return -1;
    }

    enum AVPixelFormat src_pixfmt = AV_PIX_FMT_YUV420P;
    int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));
    LOGE(" SRC _ BPP %d " , src_bpp); //12

    const uint8_t *src_data[4];

    int src_linesize[4];

    uint8_t *dst_data[4];
    int dst_linesize[4];

    int rescale_method = SWS_BICUBIC;

    struct SwsContext *img_convert_ctx;
    uint8_t *temp_buffer = (uint8_t *) malloc(width * height * src_bpp / 8);

    int frame_idx = 0;
    ret = av_image_alloc(src_data, src_linesize, width, height, src_pixfmt, 1);

    if (ret < 0) {
        LOGE("Could not allocate source image");
        return -1;
    }
    ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);

    //Init Method 1
    img_convert_ctx = sws_alloc_context();
//    av_opt_show2(img_convert_ctx,stdout,AV_OPT_FLAG_VIDEO_PARAM,0);
    av_opt_set_int(img_convert_ctx,"sws_flags",SWS_BICUBIC|SWS_PRINT_INFO,0);
    av_opt_set_int(img_convert_ctx,"srcw",width,0);
    av_opt_set_int(img_convert_ctx,"srch",height,0);
    av_opt_set_int(img_convert_ctx,"src_format",src_pixfmt,0);
    //'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
    av_opt_set_int(img_convert_ctx,"src_range",1,0);
    av_opt_set_int(img_convert_ctx,"dstw",dst_w,0);
    av_opt_set_int(img_convert_ctx,"dsth",dst_h,0);
    av_opt_set_int(img_convert_ctx,"dst_format",dst_pixfmt,0);
    av_opt_set_int(img_convert_ctx,"dst_range",1,0);
    sws_init_context(img_convert_ctx,NULL,NULL);

//    img_convert_ctx = sws_getContext(width, height, src_pixfmt, dst_w, dst_h, dst_pixfmt, SWS_BICUBIC|SWS_PRINT_INFO, NULL,
//                                     NULL, 0);

    while (1) {
        LOGE("read a frame !");
        if (fread(temp_buffer, 1, width * height * src_bpp / 8, iFile) != width * height * src_bpp / 8) {
            break;
        }
        switch (src_pixfmt) {
//            case AV_PIX_FMT_GRAY8:{
//                memcpy(src_data[0],temp_buffer , width * height);
//                break;
//            }
            case AV_PIX_FMT_YUV420P: {
                memcpy(src_data[0], temp_buffer, width * height);                    //Y
                memcpy(src_data[1], temp_buffer + width * height, width * height / 4);      //U
                memcpy(src_data[2], temp_buffer + width * height * 5 / 4, width * height / 4);  //V
                break;
            }
//            case AV_PIX_FMT_YUV422P:{
//                memcpy(src_data[0],temp_buffer,width * height);                    //Y
//                memcpy(src_data[1],temp_buffer+width * height,width * height/2);      //U
//                memcpy(src_data[2],temp_buffer+width * height*3/2,width * height/2);  //V
//                break;
//            }
//            case AV_PIX_FMT_YUV444P:{
//                memcpy(src_data[0],temp_buffer,width * height);                    //Y
//                memcpy(src_data[1],temp_buffer+width * height,width * height);        //U
//                memcpy(src_data[2],temp_buffer+width * height*2,width * height);      //V
//                break;
//            }
//            case AV_PIX_FMT_YUYV422:{
//                memcpy(src_data[0],temp_buffer,width * height*2);                  //Packed
//                break;
//            }
//            case AV_PIX_FMT_RGB24:{
//                memcpy(src_data[0],temp_buffer,width * height*3);                  //Packed
//                break;
//            }
            default: {
                LOGE("Not Support Input Pixel Format.\n");
                break;
            }
        }

        LOGE(" LINE SIZE %d " ,dst_linesize );
        sws_scale(img_convert_ctx, src_data, src_linesize, 0, height, dst_data, dst_linesize);
//        LOGE("Finish process frame %5d\n", frame_idx);
        frame_idx++;

        switch (dst_pixfmt) {
            case AV_PIX_FMT_GRAY8: {
                fwrite(dst_data[0], 1, dst_w * dst_h, oFile);
                break;
            }
            case AV_PIX_FMT_YUV420P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, oFile);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h / 4, oFile);               //U
                fwrite(dst_data[2], 1, dst_w * dst_h / 4, oFile);               //V
                break;
            }
            case AV_PIX_FMT_YUV422P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, oFile);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h / 2, oFile);               //U
                fwrite(dst_data[2], 1, dst_w * dst_h / 2, oFile);               //V
                break;
            }
            case AV_PIX_FMT_YUV444P: {
                fwrite(dst_data[0], 1, dst_w * dst_h, oFile);                 //Y
                fwrite(dst_data[1], 1, dst_w * dst_h, oFile);                 //U
                fwrite(dst_data[2], 1, dst_w * dst_h, oFile);                 //V
                break;
            }
            case AV_PIX_FMT_YUYV422: {
                fwrite(dst_data[0], 1, dst_w * dst_h * 2, oFile);               //Packed
                break;
            }
            case AV_PIX_FMT_RGB24: {
                fwrite(dst_data[0], 1, dst_w * dst_h * 3, oFile);               //Packed
                break;
            }
            default: {
                LOGE("Not Support Output Pixel Format.\n");
                break;
            }
        }
    }
    sws_freeContext(img_convert_ctx);
    free(temp_buffer);
    fclose(oFile);
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);
    LOGE("swscale ending ...%d ", frame_idx);
    return ret;
}