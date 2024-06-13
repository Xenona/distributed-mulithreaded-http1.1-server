#include "server.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "threadPool.h"
#include "utils.h"
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <algorithm>

namespace http
{

    Server::Server(std::vector<std::string> ips, std::string servIp, int port, ThreadPool *pool) : servPort(port),
                                                                                                   servSockAddrLen(sizeof(servSockAddr)), allyServers(ips), pool(pool)
    {

        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());
        memset(servSockAddr.sin_zero, '\0', sizeof servSockAddr.sin_zero);
        servSockAddrLen = sizeof(servSockAddr);

        std::vector<sockaddr_in> sockAddrs;
        for (std::string &ip : ips)
        {
            std::cout << "Friend server ip " << ip << std::endl;
            sockaddr_in sockaddr;
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_port = htons(8080);
            sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
            memset(sockaddr.sin_zero, '\0', sizeof sockaddr.sin_zero);
            sockAddrs.push_back(sockaddr);

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
                erroredExit("Couldn't open a socket");
            const int enable = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
                erroredExit("Couldn't set SO_REUSEADDR");

            if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
                erroredExit("Couldn't set SO_REUSEPORT");

            int res = connect(sock, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
            if (res == 0)
            {
                std::cout << "Server connected successfully " << std::endl;
                interserverSockets.push_back(sock);
            }
            else
            {
                std::cout << "Error: " << errno << std::endl;
            }
        }
    }

    Server::~Server()
    {
        close(servSock);
        exit(0);
    }

    std::string Server::sendMessageAndGetResponce(std::string ip, SubServerMessage message, std::string resourceName)
    {
        // std::ostream msg;

        return "";
    }

    int Server::runSocket(std::string servIp, int port, sockaddr_in &servSockAddr, unsigned int &servSockAddrLen, __socket_type sockType)
    {
        memset(&servSockAddr, '\0', sizeof(sockaddr_in));
        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());

        int servSock = socket(AF_INET, sockType, 0);
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
                std::cout << "Task enqueued" << std::endl;
                std::thread thr(&Server::handleUserConnection, this, std::ref(connSock));

                thr.join();
                close(connSock);

                std::cout << "Task completed" << std::endl; });
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
                std::string str(buffer);

                auto tokens = getReqTokens(str);

                std::string method;
                std::string url;

                for (int i = 0; i < tokens.size(); i++)
                {
                    std::string s = tokens[i];
                    if (s == "GET" || s == "POST" || s == "DELETE" || s == "UPDATE")
                    {
                        method = s;
                        url = tokens[i + 1];
                        if (url[url.length() - 1] == '/')
                            url.erase(url.length() - 1);
                    }
                }

                log("Recieved content:");
                log(str);
                std::cout << "METHOD AND URL" << method << url;
                sendResponse(connSock, method, url, tokens);
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

    void Server::sendResponse(int &connSock, const std::string &requestMethod, const std::string &requestURI, std::vector<std::string> tokens)
    {
        long bytesSent;
        std::ostringstream response;

        if (requestMethod == "GET")
        {

            getFile(response, requestURI, tokens);
        }
        else if (requestMethod == "POST")
        {
            std::string body = getReqBody(tokens);
            postFile(response, requestURI, body);
        }
        else if (requestMethod == "DELETE")
        {
        }
        else if (requestMethod == "UPDATE")
        {
        }
        else
        {
            response << "HTTP/1.1 405 Method Not Allowed\r\n"
                     << "Content-Length: 0\r\n"
                     << "Allow: GET, POST, DELETE, UPDATE\r\n"
                     << "Connection: close\r\n"
                     << "\r\n";
        }

        bytesSent = write(connSock, response.str().c_str(), response.str().size());
        if (bytesSent == response.str().size())
            log("Sent the response successfully");
        else
            log("Error sending response to client");
    }

    void Server::getFile(std::ostringstream &response, const std::string &requestURI, std::vector<std::string> tokens)
    {
        std::string filePath = "." + requestURI;

        struct stat fileStat;
        if (stat(filePath.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode))
        {
            std::ifstream file(filePath, std::ios::binary);
            if (file)
            {
                std::ostringstream fileContent;
                fileContent << file.rdbuf();
                std::string content = fileContent.str();
                response << "HTTP/1.1 200 OK\r\n"
                         << "Content-Type: " << getContentType(filePath) << "\r\n"
                         << "Content-Length: " << content.size() << "\r\n"
                         << "Last-Modified: " << getLastModifiedTime(filePath) << "\r\n"
                         << "Connection: close\r\n"
                         << "\r\n"
                         << content;
            }
        }
        else
        {

            if (std::find(tokens.begin(), tokens.end(), "Internal") != tokens.end())
            {
                std::string errorMessage = "File not found: " + requestURI;
                response << "HTTP/1.1 404 Not Found\r\n"
                         << "Content-Type: text/plain\r\n"
                         << "Content-Length: " << errorMessage.size() << "\r\n"
                         << "Connection: close\r\n"
                         << "Internal: " << requestURI << "\r\n"
                         << "\r\n"
                         << errorMessage;
            }
            else
            {
                std::string content = "";

                for (int i = 0; i < interserverSockets.size(); i++)
                {
                    std::ostringstream interserverReq;

                    interserverReq << "GET " << requestURI << " HTTP/1.1" << "\r\n"
                                   << "Content-Type: text/plain" << "\r\n"
                                   << "Accept: */*" << "\r\n"
                                   << "Host : " << allyServers[i] << ":8080" << "\r\n"
                                   << "Connection: keep-alive" << "\r\n"
                                   << "Internal : hi" << "\r\n\r\n";

                    int bytesSent = write(interserverSockets[i], interserverReq.str().c_str(), interserverReq.str().size());
                    if (bytesSent == interserverReq.str().size())
                        log("Sent the response successfully");
                    else
                        log("Error sending response to client");

                    char buffer[BUFFER_SIZE] = {'\0'};

                    int bytesRecieved = read(interserverSockets[i], buffer, BUFFER_SIZE);

                    if (bytesRecieved > 0)
                    {

                        std::string str(buffer);

                        auto tokens = getReqTokens(str);

                        auto body = getReqBody(tokens);

                        if (body != "")
                        {
                            content = body;
                        }
                    }
                }

                if (content != "")
                {
                    response << "HTTP/1.1 200 OK\r\n"
                             << "Content-Type: " << getContentType(filePath) << "\r\n"
                             << "Content-Length: " << content.size() << "\r\n"
                             << "Last-Modified: " << getLastModifiedTime(filePath) << "\r\n"
                             << "Connection: close\r\n"
                             << "\r\n"
                             << content;
                }
                else
                {
                    std::string errorMessage = "File not found: " + requestURI;
                    response << "HTTP/1.1 404 Not Found\r\n"
                             << "Content-Type: text/plain\r\n"
                             << "Content-Length: " << errorMessage.size() << "\r\n"
                             << "Connection: close\r\n"
                             << "Internal: " << requestURI << "\r\n"
                             << "\r\n"
                             << errorMessage;
                }
            }
        }
    }

    std::string Server::getContentType(const std::string &fileName)
    {
        std::string extension = fileName.substr(fileName.find_last_of('.') + 1);
        if (extension == "html" || extension == "htm")
            return "text/html";
        else if (extension == "txt")
            return "text/plain";
        else if (extension == "jpg" || extension == "jpeg")
            return "image/jpeg";
        else if (extension == "png")
            return "image/png";
        else if (extension == "pdf")
            return "application/pdf";
        else
            return "application/octet-stream";
    }

    std::string Server::getLastModifiedTime(const std::string &fileName)
    {
        struct stat fileStat;
        if (stat(fileName.c_str(), &fileStat) != 0)
        {
            return "";
        }

        std::time_t lastModifiedTime = fileStat.st_mtime;
        std::tm *tmPtr = std::gmtime(&lastModifiedTime);
        char buf[80];
        std::strftime(buf, 80, "%a, %d %b %Y %H:%M:%S GMT", tmPtr);
        return buf;
    }

    void Server::postFile(std::ostringstream &response, const std::string &requestURI, const std::string &body)
    {
        std::string filePath = "." + requestURI;

        if (filePath.empty())
        {
            formErrorResponse(400, "Bad Request", "Filename not found in URI", response);
            return;
        }
        std::ofstream outFile(filePath);
        if (!outFile.is_open())
        {
            formErrorResponse(500, "Internal Server Error", "Error opening an output file", response);
            return;
        }

        outFile << body;
        outFile.close();

        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << getContentType(filePath) << "\r\n"
                 << "Content-Length: " << 0 << "\r\n"
                 << "Last-Modified: " << getLastModifiedTime(filePath) << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n";
    };

    void Server::formErrorResponse(int statusCode, const std::string &message, const std::string &statusText, std::ostringstream &response)
    {
        response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n"
                 << "Content-Type: text/plain\r\n"
                 << "Content-Length: " << message.size() << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n"
                 << message;
    }

}
