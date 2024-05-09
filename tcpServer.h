#pragma once
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

namespace http
{

    class ThreadPool;

    class TcpServer
    {
    public:
        TcpServer(std::vector<std::string> ips, std::string servIp, int port, ThreadPool* pool);
        ~TcpServer();
        void startListen();
        int acceptConnection();
        void handleUserConnection(int &connSock);
        void sendResponse();

    private:
        std::vector<std::string> friendServers;
        int servSock;
        int servPort;
        int connSock;
        struct sockaddr_in servSockAddr;
        unsigned int servSockAddrLen;
        ThreadPool* pool;
    };
}