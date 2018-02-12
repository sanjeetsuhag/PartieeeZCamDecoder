//
// Created by Albert Xu on 2/11/18.
//

#include "opencv2/opencv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifndef ZCAMSTREAM_STREAM_DECODER_H
#define ZCAMSTREAM_STREAM_DECODER_H

class StreamDecoder
{
private:
    int sokt;
    char ping=0x01;
    uchar* buffer;
    cv::Mat image;
    size_t cur_buf_size;
    uint expected_buf_size = 0;

    bool save_frames = false;
    FILE* f;

public:
    StreamDecoder(const char* serverIP, int serverPort);
    void grab_image();
    void record_images();
    void start_stream();
    void stop_stream();
};

#endif //ZCAMSTREAM_STREAM_DECODER_H
