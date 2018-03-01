//
// Created by Albert Xu on 2/27/18.
//

#include "ffmpeg.h"

ffmpeg::ffmpeg(AVCodecID decoder)
{
    codec = avcodec_find_decoder(decoder);
    codec_context = avcodec_alloc_context3(codec);
    avcodec_open2(codec_context,codec,0);
    frame = av_frame_alloc();
}

ffmpeg::~ffmpeg()
{
    av_free(frame);
    avcodec_close(codec_context);
}

int ffmpeg::decode_frame(unsigned char* buffer, size_t sz)
{
    AVPacket *packet = new AVPacket();
    av_init_packet(packet);

    packet->data = buffer;
    packet->size = (int)sz;

    int ret;
//    int ret = decode(codec_context, frame, &got_frame, packet);

    ret = avcodec_send_packet(codec_context, packet);
    if(ret < 0)
    {
        char errbuf[99];
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret == AVERROR_EOF ? 0 : ret;
    }

    ret = avcodec_receive_frame(codec_context, frame);
    if (ret < 0 && ret != AVERROR_EOF)
    {
        char errbuf[99];
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret;
    }

//    img_convert((AVPicture *)frameRGB, PIX_FMT_RGB24, (AVPicture*)frame,
//                codec_context->pix_fmt, codec_context->width, codec_context->height);

    return 0;
}

cv::Mat ffmpeg::retrieve_frame()
{
//    AVFrame *frameRGB = avcodec_alloc_frame();

    cv::Mat mat(codec_context->height, codec_context->width,
                CV_8UC3);
//, frame->data[0], (size_t)frame->linesize[0]);
    return mat;
}

//int ffmpeg::decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
//{
//    int ret;
//
//    *got_frame = 0;
//
//    if (pkt) {
//        ret = avcodec_send_packet(avctx, pkt);
//        // In particular, we don't expect AVERROR(EAGAIN), because we read all
//        // decoded frames with avcodec_receive_frame() until done.
//        if (ret < 0)
//            return ret == AVERROR_EOF ? 0 : ret;
//    }
//
//    ret = avcodec_receive_frame(avctx, frame);
//    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
//        return ret;
//    if (ret >= 0)
//        *got_frame = 1;
//
//    return 0;
//}

//void ffmpeg::decodeStreamData(unsigned char * pData, size_t sz)
//{
//    AVPacket        packet;
//    av_init_packet(&packet);
//
//    packet.data=pData;
//    packet.size=(int)sz;
//    int framefinished=0;
//    int nres=avcodec_decode_video2(codec_context,frame,&framefinished,&packet);
//
//    if(framefinished)
//    {
//        // do the yuv magic and call a consumer
//    }
//
//    return;
//}