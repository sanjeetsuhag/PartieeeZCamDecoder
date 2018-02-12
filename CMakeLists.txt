cmake_minimum_required(VERSION 3.9)
project(ZCamStream)

set(CMAKE_CXX_STANDARD 11)


find_package(OpenCV REQUIRED)
#find_package(AVFORMAT)
#find_package(libavcodec)
#find_package(libavutil/avutil.h)
#include_directories(
#        ${OpenCV_INCLUDE_DIRS}
#        ${AVFORMAT_INCLUDE_DIR}
#        ${AVCODEC_INCLUDE_DIR}
#        ${AVUTIL_INCLUDE_DIR}
#)

add_executable(TCP_stream_server server.cpp)
target_link_libraries(TCP_stream_server ${OpenCV_LIBS})

add_executable(TCP_stream_client client.cpp)
target_link_libraries(TCP_stream_client ${OpenCV_LIBS})

add_library(streamdecode SHARED stream_decoder.cpp)
target_link_libraries(streamdecode ${OpenCV_LIBS})

add_executable(Zcam_client ZCam_client.cpp)
target_link_libraries(Zcam_client ${OpenCV_LIBS})
target_link_libraries(Zcam_client streamdecode)

add_executable(ZCam_server_sim ZCam_server_sim.cpp)
target_link_libraries(ZCam_server_sim ${OpenCV_LIBS})

set(
        IMGPROC_LIBRARIES
        ${OpenCV_LIBS}
        ${AVFORMAT_LIBRARY}
        ${AVCODEC_LIBRARY}
        ${AVUTIL_LIBRARY}
)

add_executable(decode_ex h264_decode_ex.cpp)
target_link_libraries(decode_ex IMGPROC_LIBRARIES)
