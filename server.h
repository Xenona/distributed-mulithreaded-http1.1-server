#pragma once
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

namespace http
{

    enum class SubServerMessage : int
    {
        getResourse = 0,
        sendResourse,
        resourceSize,
    };

    class ThreadPool;

    class Server
    {
    public:
        Server(std::vector<std::string> ips, std::string servIp, int port, ThreadPool *pool);
        ~Server();
        void startListen();
        int acceptConnection();
        void handleUserConnection(int &connSock);
        void sendResponse(int &connSock, const std::string &requestMethod, const std::string &requestURI);
        static int runSocket(std::string servIp, int port, sockaddr_in &servSockAddr, unsigned int &servSockAddrLen, __socket_type sockType);
        ThreadPool *pool;

    private:
        std::vector<std::string> allyServers;
        int servSock;
        int servPort;
        struct sockaddr_in servSockAddr;
        unsigned int servSockAddrLen;
        void handleFileRequest(std::ostringstream &response, const std::string &requestURI);
        std::string getContentType(const std::string &fileName);
        std::string getLastModifiedTime(const std::string &fileName);
        std::string sendMessageAndGetResponce(std::string ip, SubServerMessage message, std::string resourceName);
    };
}