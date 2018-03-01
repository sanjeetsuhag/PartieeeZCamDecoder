//
// Created by Albert Xu on 2/27/18.
//

#ifndef ZCAMSTREAM_FFMPEG_H
#define ZCAMSTREAM_FFMPEG_H

#include <unistd.h>
#include "opencv2/opencv.hpp"

extern"C"{
#define __STDC_CONSTANT_MACROS
#include"libavcodec/avcodec.h"
#include"libavformat/avformat.h"
#include"libavutil/error.h"
#include"libavutil/imgutils.h"
#include"libswscale/swscale.h"
}

class ffmpeg
{
public:
    explicit ffmpeg(AVCodecID decoder);
    ~ffmpeg();
//    void decodeStreamData(unsigned char * pData, size_t sz);
//    int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt);
    int decode_frame(unsigned char* buffer, size_t sz);
    cv::Mat retrieve_frame();

//protected:
    AVCodecContext  *codec_context;
    AVCodec         *codec;
    AVFrame         *frame;
};


#endif //ZCAMSTREAM_FFMPEG_H
