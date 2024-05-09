cmake_minimum_required(VERSION 3.10)

project(http-servers VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)



add_executable(http-servers 
main.cpp
server.cpp
httpServer.cpp
threadPool.cpp
utils.cpp
nftables.cpp
)

target_link_libraries(http-servers Threads::Threads /usr/lib/libnftables.so.1.1.0)
target_include_directories(http-servers PRIVATE .)

set(SOURCE_FILE ${CMAKE_SOURCE_DIR}/serverIpList.txt)
set(DEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/serverIpList.txt)

add_custom_command(
    TARGET http-servers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${DEST_FILE}
    COMMENT "Copying serverIpList.txt to build directory"
)

add_custom_target(copy_server_ip_list ALL DEPENDS ${DEST_FILE})