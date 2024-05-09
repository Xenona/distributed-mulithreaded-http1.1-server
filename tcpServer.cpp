#include "tcpServer.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "threadPool.h"

using namespace std;

namespace
{
    const int BUFFER_SIZE = 32768;

    void log(const string &message)
    {
        cout << message << endl;
    }

    void erroredExit(const string &errMessage)
    {
        log("ERROR: " + errMessage);
        exit(1);
    }

}
namespace http
{

    TcpServer::TcpServer(vector<string> ips, string servIp, int port, ThreadPool *pool) : servPort(port), servSockAddrLen(sizeof(servSockAddr)), friendServers(ips), pool(pool)
    {

        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());

        servSock = socket(AF_INET, SOCK_STREAM, 0);
        if (servSock < 0)
            erroredExit("Couldn't open a socket");

        const int enable = 1;
        if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            erroredExit("Couldn't set SO_REUSEADDR");
        }

        if (setsockopt(servSock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
            erroredExit("Couldn't set SO_REUSEPORT");
        }

        if (bind(servSock, (sockaddr *)&servSockAddr, servSockAddrLen) < 0)
            erroredExit("Couldn't bind the socket to address");
    }

    TcpServer::~TcpServer()
    {
        close(servSock);
        close(connSock);
        exit(0);
    }

    void TcpServer::startListen()
    {
        if (listen(servSock, 20) < 0)
            erroredExit("Could not listen on the socket");

        log("Listening...");

        while (true)
        {

            connSock = acceptConnection();
            // thread thr(&TcpServer::handleUserConnection, this, ref(connSock));

            pool->enqueueTask([this]
                              {
                                  cout << "HIIIIIIIIIIII" << endl;
                                  thread thr(&TcpServer::handleUserConnection, this, ref(connSock));

                                  thr.join();
                                  cout << "BYEEEEEEEEEEEE" << endl; });
        }
    }

    void TcpServer::handleUserConnection(int &connSock)
    {
        fd_set sock;
        FD_ZERO(&sock);
        FD_SET(connSock, &sock);

        while (true)
        {

            fd_set s = sock;
            int sel = select(connSock + 1, &s, 0, 0, 0);

            if (sel < 0)
                erroredExit("Select broke");

            int bytesRecieved;

            char buffer[BUFFER_SIZE] = {'\0'};

            bytesRecieved = read(connSock, buffer, BUFFER_SIZE);

            if (bytesRecieved < 0)
                erroredExit("Could not recieve bytes from a remote socket");
            else if (bytesRecieved == 0)
            {
                log("Client disconnected");
                close(connSock);
                break;
            }
            else // bytesRecieved > 0
            {
                string str(buffer);

                log("Recieved content:");
                log(str);
                sendResponse();
            }
        }
    }

    int TcpServer::acceptConnection()
    {
        int connSock = accept(servSock, (sockaddr *)&servSockAddr, &servSockAddrLen);

        if (connSock < 0)
            erroredExit("Could not accept connection");

        return connSock;
    }

    void TcpServer::sendResponse()
    {
        long bytesSent;

        std::string json = "{\"status\": \"ok\"}";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/json\nContent-Length: " << json.size() << "\n\n"
           << json;

        bytesSent = write(connSock, ss.str().c_str(), ss.str().size());

        if (bytesSent == ss.str().size())
            log("Sent the response successfully");
        else
            log("Error sending response to client");
    }

}