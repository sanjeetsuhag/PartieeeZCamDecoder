/**
 * OpenCV video streaming over TCP/IP
 * Client: Receives video from server and display it
 * by Steve Tuenkam
 */

#include "opencv2/opencv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "stream_decoder.h"

using namespace cv;


int main(int argc, char** argv)
{
    //--------------------------------------------------------
    //networking stuff: socket , connect
    //--------------------------------------------------------
    char* serverIP;
    int   serverPort;

    if (argc < 3) {
        std::cerr << "Usage: cv_video_cli <serverIP> <serverPort> " << std::endl;
    }

    serverIP   = argv[1];
    serverPort = atoi(argv[2]);

    StreamDecoder stream(serverIP, serverPort);
//    stream.record_images();
    stream.start_stream();
}