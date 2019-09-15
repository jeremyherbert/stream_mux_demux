cmake_minimum_required(VERSION 3.14)
project(stream_mux_demux C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -O0)

add_executable(stream_mux_demux stream_demux.c stream_mux.c main.cpp)