cmake_minimum_required(VERSION 3.9)
project(ZCamStream)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_CURRENT_SOURCE_DIR}/FindFFMpeg.cmake)

find_package(OpenCV REQUIRED)
#find_package(FFMpeg REQUIRED)

add_executable(TCP_stream_server server.cpp)
target_link_libraries(TCP_stream_server ${OpenCV_LIBS})

add_executable(TCP_stream_client client.cpp)
target_link_libraries(TCP_stream_client ${OpenCV_LIBS})

add_library(ffmpeg SHARED ffmpeg.cpp)
target_link_libraries(ffmpeg ${FFMPEG_LIBRARIES} ${OpenCV_LIBS})

add_library(streamdecode SHARED stream_decoder.cpp)
target_link_libraries(streamdecode ${OpenCV_LIBS} ffmpeg)

add_executable(ZCam_client ZCam_client.cpp)
target_link_libraries(ZCam_client ${OpenCV_LIBS})
target_link_libraries(ZCam_client streamdecode)

add_executable(ZCam_server_sim ZCam_server_sim.cpp)
target_link_libraries(ZCam_server_sim ${OpenCV_LIBS})

add_executable(vid_stab videostab.cpp)
target_link_libraries(vid_stab ${OpenCV_LIBS})

add_executable(akaze_stab akaze_stab.cpp)
target_link_libraries(akaze_stab ${OpenCV_LIBS})

add_executable(ffmpeg_test ffmpeg_test.cpp)
target_link_libraries(ffmpeg_test ${OpenCV_LIBS} ffmpeg)
