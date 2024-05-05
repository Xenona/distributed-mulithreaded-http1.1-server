cmake_minimum_required(VERSION 3.10)

project(http-servers VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(http-servers 
main.cpp
tcpServer.cpp
)

target_include_directories(http-servers PRIVATE .)

