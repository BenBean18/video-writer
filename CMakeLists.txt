cmake_minimum_required(VERSION 3.14)
project(vwriter C CXX)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/FFmpeg)

list(APPEND SOURCES
    src/main.cpp
    src/new_video_writer.cpp
    src/new_video_writer.h
)
#MACOSX_BUNDLE WIN32 

add_library(video_writer SHARED src/vwutils.h src/vwutils.cpp)

target_link_libraries(video_writer FFmpeg)

add_executable(vwriter ${SOURCES})

target_link_libraries(vwriter FFmpeg)

add_executable(encode_video src/encode_video.cpp)

target_link_libraries(encode_video FFmpeg)