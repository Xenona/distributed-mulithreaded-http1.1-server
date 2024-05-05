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
        int startServer();
        void startListen();
        void acceptConnection(int &connSock);

    private:
        int servSock;
        int servPort;
        int connSock;
        struct sockaddr_in servSockAddr;
        unsigned int servSockAddrLen;
    };
}