#pragma once
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

namespace http
{

    class ThreadPool;

    class Server
    {
    public:
        Server(std::vector<std::string> ips, std::string servIp, int port, ThreadPool *pool);
        ~Server();
        void startListen();
        int acceptConnection();
        void handleUserConnection(int &connSock);
        void sendResponse(int &connSock);
        static int runSocket(std::string servIp, int port, sockaddr_in &servSockAddr, unsigned int &servSockAddrLen);
        ThreadPool *pool;

    private:
        std::vector<std::string> friendServers;
        int servSock;
        int servPort;
        struct sockaddr_in servSockAddr;
        unsigned int servSockAddrLen;
    };
}