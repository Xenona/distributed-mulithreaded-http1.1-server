#include "httpServer.h"
#include <iostream>
#include "utils.h"
#include "threadPool.h"

using namespace std;
namespace http
{

    HttpServer::HttpServer(Server &serverInstance, std::string servIp, int port) : Server(serverInstance)
    {
        httpServSockAddrLen = sizeof(httpServSockAddr);
        httpServSock = runSocket(servIp, port, httpServSockAddr, httpServSockAddrLen);
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
                cout << "HIIIIIIIIIIII" << endl;
                thread thr(&Server::handleUserConnection, this, std::ref(connSock));

                thr.join();
                close(connSock);


                cout << "BYEEEEEEEEEEEE" << endl; });
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