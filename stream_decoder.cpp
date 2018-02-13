//
// Created by Albert Xu on 2/11/18.
//

#include "stream_decoder.h"

void reverse_array(uchar* arr, size_t len)
{
    for(size_t i=0; i < (len/2); ++i)
    {
        uchar temp = arr[i];
        arr[i] = arr[len-i-1];
        arr[len-i-1] = temp;
    }
}

StreamDecoder::StreamDecoder(const char* serverIP, int serverPort)
{

    // tbh no clue what this statement does.
    struct sockaddr_in serverAddr;

    socklen_t addrLen = sizeof(struct sockaddr_in);

    if ((sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket() failed" << std::endl;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0)
    {
        std::cerr << "connect() failed!" << std::endl;
    }
}

void StreamDecoder::start_stream()
{
    // This `key` doesnt do anything because I removed the cv window. It's kinda unfortunate.
    int key=0;
    while (key != 'q')
    {
        grab_image();
        if ((key = cv::waitKey(10)) >= 0) break;
    }

    stop_stream();
}

void StreamDecoder::stop_stream()
{
    close(sokt);
}

void StreamDecoder::grab_image()
{
    std::clog << "Pinging for an image" << std::endl;
    send(sokt, &ping, sizeof(ping), 0);
    uchar size_bytes[4];
    recv(sokt, size_bytes, 4, 0);

    if(save_frames) fwrite(size_bytes, sizeof(uchar), 4, f);

    // I have to reverse the byte order b/c my computer is little endian. (stream comes in big endian)
    reverse_array((uchar*)size_bytes,4);
    memcpy(&expected_buf_size, size_bytes, sizeof(expected_buf_size));

    std::clog << "Expected size: " << expected_buf_size << std::endl;

    // Prints the bytes of `size_bytes`
//    auto *p = (unsigned char *)&size_bytes;
//    size_t i;
//    for (i=0; i < sizeof size_bytes; ++i)
//        printf("\\x%02x", p[i]);
//    std::cout << std::endl;

    cur_buf_size = 0;
    buffer = new uchar[expected_buf_size];

    //TODO: Implement a timeout for this loop
    while(cur_buf_size < expected_buf_size)
    {
        auto bytes = recv(sokt, buffer+cur_buf_size, expected_buf_size-(size_t)cur_buf_size, 0);
        if(bytes == -1) continue;
        cur_buf_size += bytes;
    }

    //TODO: Check for any extraneous data in buffer. If there is, read it all and flush the current buffer.

    std::clog << "Recieved size: " << cur_buf_size << std::endl;

    if(save_frames) fwrite(buffer, sizeof(uchar), cur_buf_size, f);
}

void StreamDecoder::record_images(const char* filename)
{
    f = fopen(filename, "wb");
    save_frames = true;
}
