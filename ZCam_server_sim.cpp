//
// Created by Albert Xu on 2/11/18.
//

#include "opencv2/opencv.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

FILE* f;

void reverse_array(u_char* arr, size_t len)
{
    for(size_t i=0; i < (len/2); ++i)
    {
        u_char temp = arr[i];
        arr[i] = arr[len-i-1];
        arr[len-i-1] = temp;
    }
}

void* send_data(void *ptr)
{
    int socket = *(int *)ptr;

    int dummy=0;
    uchar size_bytes[4];

    uint size;
    uchar* send_data;

    while(true)
    {
        recv(socket, &dummy, 1, 0);
        std::cout << "Recieved ping" << std::endl;

        if(feof(f)) break;
        fread(size_bytes, 1, 4, f);
        send(socket, size_bytes, 4, 0);
        std::cout << "Sent size bytes:" << std::endl;

        auto *p = (uchar *)&size_bytes;
        size_t i;
        for (i=0; i < sizeof(size_bytes); ++i)
            printf("%02x ", p[i]);
        std::cout << std::endl;

        reverse_array((u_char*)size_bytes,4);
        memcpy(&size, &size_bytes, sizeof(size));

        std::cout << "Size: " << size << std::endl;

        send_data = new u_char[size];
        if(feof(f)) break;
        fread(send_data, size, 1, f);
        send(socket, send_data, size, 0);
        std::cout << "Sent data bytes." << std::endl;
    }
}

int main(int argc, char** argv)
{
//    u_char size_bytes[4] = {0x00, 0x00, 0x03, 0x3e};
//    uint size = 0;
//
//    auto *p = (uchar *)&size_bytes;
//    size_t i;
//    for (i=0; i < sizeof(size_bytes); ++i)
//        printf("%02x ", p[i]);
//    std::cout << std::endl;
//
//    memcpy(&size, size_bytes, sizeof(size));
//    std::cout << size << std::endl;
//
//    auto *p2 = (uchar *)&size;
//    size_t i2;
//    for (i2=0; i2 < sizeof(size); ++i2)
//        printf("%02x ", p2[i2]);
//    std::cout << std::endl;


    int localSocket,
        remoteSocket,
        port = 9876;

    struct  sockaddr_in localAddr,
                        remoteAddr;
    pthread_t thread_id;

    int addrLen = sizeof(struct sockaddr_in);

    if (argc!=3 && argc!=2) {
        std::cerr << "usage: ./ZCam_server_sim [fname] [port]\n" <<
                  "fname: binary file name\n" <<
                  "port:  socket port (9876 default)\n" << std::endl;

        exit(1);
    }

    f = fopen(argv[1], "rb");
    if(argc == 3)
    {
        port = atoi(argv[2]);
    }

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (localSocket == -1){
        perror("socket() call failed!!");
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons( port );

    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
        perror("Can't bind() socket");
        exit(1);
    }

    //Listening
    listen(localSocket , 3);

    std::cout << "localSocket " << localSocket << std::endl;
    std::cout <<  "Waiting for connections...\n"
              <<  "Server Port: " << port << std::endl;


    //accept connection from an incoming client
    while(1){
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);
        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        }
        std::cout << "Connection accepted" << std::endl;
        std::cout << "remoteSocket " << remoteSocket << std::endl;
        pthread_create(&thread_id,NULL,send_data,&remoteSocket);

    }
    //pthread_join(thread_id,NULL);
    //close(remoteSocket);

    return 0;
}
