cmake_minimum_required(VERSION 3.10)

project(http-servers)

add_executable(http-servers main.cpp tcpServer.cpp tcpServer.h)

target_include_directories(http-servers PRIVATE .)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)