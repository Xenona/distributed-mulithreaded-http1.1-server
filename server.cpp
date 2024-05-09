#include "server.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "threadPool.h"
#include "utils.h"
using namespace std;

 
namespace http
{

    Server::Server(vector<string> ips, string servIp, int port, ThreadPool *pool) : servPort(port), servSockAddrLen(sizeof(servSockAddr)), friendServers(ips), pool(pool)
    {
        for (string &ip : ips)
        {
            cout << "Friend server ip " << ip << endl;
        }

        servSock = runSocket(servIp, port, servSockAddr, servSockAddrLen);
    }

    Server::~Server()
    {
        close(servSock);
        exit(0);
    }

    int Server::runSocket(string servIp, int port, sockaddr_in &servSockAddr, unsigned int &servSockAddrLen)
    {

        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());

        int servSock = socket(AF_INET, SOCK_STREAM, 0);
        if (servSock < 0)
            erroredExit("Couldn't open a socket");

        const int enable = 1;
        if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            erroredExit("Couldn't set SO_REUSEADDR");

        if (setsockopt(servSock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
            erroredExit("Couldn't set SO_REUSEPORT");

        if (bind(servSock, (sockaddr *)&servSockAddr, servSockAddrLen) < 0)
            erroredExit("Couldn't bind the socket to address");

        return servSock;
    }

    void Server::startListen()
    {
        if (listen(servSock, 20) < 0)
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

    void Server::handleUserConnection(int &connSock)
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
                sendResponse(connSock);
            }
        }
    }

    int Server::acceptConnection()
    {
        int connSock = accept(servSock, (sockaddr *)&servSockAddr, &servSockAddrLen);

        if (connSock < 0)
            erroredExit("Could not accept connection");

        return connSock;
    }

    void Server::sendResponse(int &connSock)
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