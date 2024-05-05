#include "tcpServer.h"
#include <iostream>
#include <unistd.h>

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
    TcpServer::TcpServer(vector<string> ips, string servIp, int port): servPort(port), servSockAddrLen(sizeof(servSockAddr))
    {

        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());

        startServer();
    }

    TcpServer::~TcpServer()
    {
        close(servSock);
        close(connSock);
        exit(0);

    }

    int TcpServer::startServer()
    {
        servSock = socket(AF_INET, SOCK_STREAM, 0);
        if (servSock < 0)
            erroredExit("Couldn't open a socket");

        if (bind(servSock, (sockaddr *)&servSockAddr, servSockAddrLen) < 0)
            erroredExit("Couldn't bind the socket to address");
        
        return 0;
    }

    void TcpServer::startListen() {
        if (listen(servSock, 20) < 0) 
            erroredExit("Could not listen on the socket");

        log("Listening...");

        int bytesRecieved;

        while (true) {
            acceptConnection(connSock);

            char buffer[BUFFER_SIZE] = {'\0'};

            bytesRecieved = read(connSock, buffer, BUFFER_SIZE);
            if (bytesRecieved < 0) 
                erroredExit("Could not recieve bytes from a remote socket");

            string str(buffer);

            log(str);
            

        }
    
    }

    void TcpServer::acceptConnection(int &connSock) {
        connSock = accept(servSock, (sockaddr *)&servSockAddr, &servSockAddrLen);

        if (connSock < 0) 
            erroredExit("Could not accept connection");
    }

     

}