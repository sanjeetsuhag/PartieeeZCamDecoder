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

    bool recording = false;
    const char* filename;

    if (argc < 3) {
        std::cerr << "Usage:\t./Zcam_client [-r fname] <serverIP> <serverPort>\n"
                "[-r]:\tSpecify flag to record the stream with [-r] or [--record]. Accepts a filename afterwards.\n"
                "serverIP:\tThe IP to connect to\n"
                "serverPort:\tThe port to connect to\n";
        return 1;
    }

    while(argv[1][0] == '-')
    {
        std::string str(argv[1]);
        if((str=="-r") || (str=="--record"))
        {
            recording=true;
            filename = argv[2];
            argv += 2;
        }
        else
        {
            std::cout << "Unrecognized flag: " << str << std::endl;
            argv += 1;
        }
    }

    serverIP   = argv[1];
    serverPort = atoi(argv[2]);

    StreamDecoder stream(serverIP, serverPort);
    if(recording) stream.record_images(filename);
    stream.start_stream();
}