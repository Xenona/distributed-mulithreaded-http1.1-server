#include "httpServer.h"
#include "utils.h"
#include "threadPool.h"
namespace http
{

    HttpServer::HttpServer(Server &serverInstance, std::string servIp, int port) : Server(serverInstance)
    {
        httpServSockAddrLen = sizeof(httpServSockAddr);
        httpServSock = runSocket(servIp, port, httpServSockAddr, httpServSockAddrLen, SOCK_STREAM);
    }

    HttpServer::~HttpServer()
    {
    }

    void HttpServer::startListen()
    {
        if (listen(httpServSock, 20) < 0)
            erroredExit("Could not listen on the socket");

        log("Listening...");

        while (true)
        {

            int connSock = acceptConnection();
            pool->enqueueTask([this, &connSock]
                              {
                                  std::thread thr(&Server::handleUserConnection, this, std::ref(connSock));

                                  thr.join();
                                  close(connSock);
                              });
        }
    }

    int HttpServer::acceptConnection()
    {
        int connSock = accept(httpServSock, (sockaddr *)&httpServSockAddr, &httpServSockAddrLen);

        if (connSock < 0)
            erroredExit("Could not accept connection");

        return connSock;
    }

}