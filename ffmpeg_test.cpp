//
// Created by Albert Xu on 2/27/18.
//
#include <iostream>
#include <hdf5_hl.h>
#include "ffmpeg.h"

void reverse_array(u_char* arr, size_t len)
{
    for(size_t i=0; i < (len/2); ++i)
    {
        u_char temp = arr[i];
        arr[i] = arr[len-i-1];
        arr[len-i-1] = temp;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3 && argc != 2)
    {
        std::cerr << "usage: ./ffmpeg_test [fname]\n" <<
                  "fname: binary file name\n" << std::endl;

        exit(1);
    }

    FILE *f = fopen(argv[1], "rb");

    u_char size_bytes[4];
    uint size;
    u_char *im_data;

    char errbuf[99];

    // Read buffer in
    fread(size_bytes, 1, 4, f);
    reverse_array((u_char *) size_bytes, 4);
    memcpy(&size, &size_bytes, sizeof(size));
    im_data = new u_char[size];
    fread(im_data, size, 1, f);

    // Initialize AV stuffs.
    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    AVCodecContext *codec_context = avcodec_alloc_context3(codec);
    avcodec_open2(codec_context, codec, nullptr);
    AVFrame *frame = av_frame_alloc();

    // Decode buffer
    std::cout << "Decoding" << std::endl;
    auto *packet = new AVPacket();
    av_init_packet(packet);

    packet->data = im_data;
    packet->size = (int) size;

    int ret;
    ret = avcodec_send_packet(codec_context, packet);
    if (ret < 0)
    {
        std::cerr << "Died at send packet" << std::endl;
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret == AVERROR_EOF ? 0 : ret;
    }

    // Read 2nd image?
    fread(size_bytes, 1, 4, f);
    reverse_array((u_char *) size_bytes, 4);
    memcpy(&size, &size_bytes, sizeof(size));
    im_data = new u_char[size];
    fread(im_data, size, 1, f);

    packet->data = im_data;
    packet->size = (int) size;

    ret = avcodec_send_packet(codec_context, packet);
    if (ret < 0)
    {
        std::cerr << "Died at send packet 2" << std::endl;
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret == AVERROR_EOF ? 0 : ret;
    }

    ret = avcodec_receive_frame(codec_context, frame);
    if (ret < 0)
    {
        std::cerr << "Died at recieve_frame" << std::endl;
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret;
    }

    FILE* f2 = fopen("asdf.bin", "wb");
    int frame_size = av_image_get_buffer_size(codec_context->pix_fmt, frame->width, frame->height, 1);
    std::cout << "actual size: " << frame_size << std::endl;

    frame_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, frame->width, frame->height, 1);
    std::cout << "guess size: " << frame_size << std::endl;


    fwrite(frame->data, sizeof(frame->data), (size_t) frame_size, f2);

    //Converting to RGB?
    std::cout << "Converting" << std::endl;
    SwsContext* ctx = sws_getContext(frame->width, frame->height, AV_PIX_FMT_YUV420P,
                                     frame->width, frame->height, AV_PIX_FMT_BGR24,
                                     0, nullptr, nullptr, nullptr);

    if(ctx == nullptr) {
        fprintf(stderr, "Cannot initialize the conversion context!\n");
        exit(1);
    }

    std::cout << frame->format << std::endl;

    AVFrame* framergb = av_frame_alloc();
    framergb->width = frame->width;
    framergb->height = frame->height;
    framergb->format = AV_PIX_FMT_BGR24;

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
//    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, frame->width,frame->height);
    uint8_t* buf = new uint8_t[numBytes];

    std::cout << "NBYTES " << numBytes << std::endl;

    ret = sws_scale(ctx,
                    (const uint8_t* const *) frame->data,frame->linesize,
                    0, frame->height,
                    framergb->data, framergb->linesize);

    av_strerror(ret, errbuf, 99);
    std::cout << ret << std::endl;
    std::cout << errbuf << std::endl;

    std::cout << framergb->data << std::endl;

    std::cout << "av (w, h): (" << framergb->width << ", " << framergb->height << ")" << std::endl;
//    std::cout << "Pixel format: 0x" << std::hex << avcodec_pix_fmt_to_codec_tag(AV_PIX_FMT_BGR24) << std::endl;

    cv::Mat mat(framergb->height, framergb->width, CV_8UC3);
//    cv::Mat mat(framergb->height, framergb->width,
//                CV_8UC3, framergb->data[0], (size_t)framergb->linesize[0]);

    std::cout << "cv (w, h): (" << mat.cols << ", " << mat.rows << ")" << std::endl;

    std::cout << "Done writing?" << std::endl;
    cv::imshow("frame", mat);
    cv::waitKey(0);

    // Writing AVFrame back out
    std::cout << "Writing" << std::endl;
    auto* packet2 = new AVPacket();
    av_init_packet(packet2);

    // Formatting codec context
    AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);
    jpegContext->pix_fmt = codec_context->pix_fmt;
    jpegContext->height = frame->height;
    jpegContext->width = frame->width;

    std::cout << "(w, h): (" << frame->width << ", " << frame->height << ")\n";
    std::cout << "Pixel format: 0x" << std::hex << avcodec_pix_fmt_to_codec_tag(codec_context->pix_fmt) << std::endl;

    ret = avcodec_send_frame(jpegContext, frame);
    if (ret < 0)
    {
        std::cerr << "Died at send frame" << std::endl;
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;

        std::cout << frame->data << std::endl;

        return ret;
    }

    ret = avcodec_receive_packet(jpegContext, packet2);
    if (ret < 0)
    {
        std::cerr << "Died at receive packet" << std::endl;
        av_strerror(ret, errbuf, 99);
        std::cerr << ret << std::endl;
        std::cerr << errbuf << std::endl;
        return ret;
    }
    std::cout << "Packet size: " << packet->size << std::endl;

    FILE* JPEGFile = fopen("Test.jpeg", "wb");
    fwrite(packet->data, 1, (size_t)packet->size, JPEGFile);
}
