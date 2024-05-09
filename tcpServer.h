#pragma once
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace http
{

    class TcpServer
    {
    public:
        TcpServer(std::vector<std::string> ips, std::string servIp, int port);
        ~TcpServer();
        void startListen();
        void acceptConnection(int &connSock);
        void handleConnection(int &connSock);
        void sendResponse();

    private:
        int servSock;
        int servPort;
        int connSock;
        struct sockaddr_in servSockAddr;
        unsigned int servSockAddrLen;
    };
}