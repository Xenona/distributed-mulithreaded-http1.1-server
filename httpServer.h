#pragma once
#include "server.h"

namespace http
{
    class HttpServer : public Server
    {
    public:
        HttpServer(Server &serverInstance, std::string servIp, int port);
        ~HttpServer();
        void startListen();
        int acceptConnection();

    private:
        int httpServSock;
        int httpServPort;
        struct sockaddr_in httpServSockAddr;
        unsigned int httpServSockAddrLen;
    };
}