cmake_minimum_required(VERSION 3.10)

project(http-servers VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)



add_executable(http-servers 
main.cpp
tcpServer.cpp
threadPool.cpp
)

target_link_libraries(http-servers Threads::Threads)
target_include_directories(http-servers PRIVATE .)
