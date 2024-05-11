#include "server.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include "threadPool.h"
#include "utils.h"
#include <cstring>
#include <sys/stat.h>
#include <fstream>

using namespace std;

namespace http
{

    Server::Server(vector<string> ips, string servIp, int port, ThreadPool *pool) : servPort(port),
                                                                                    servSockAddrLen(sizeof(servSockAddr)), allyServers(ips), pool(pool)
    {
        vector<sockaddr_in> sockAddrs;
        for (string &ip : ips)
        {
            cout << "Friend server ip " << ip << endl;
            sockaddr_in sockaddr;
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_port = htons(port);
            sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
            memset(sockaddr.sin_zero, '\0', sizeof sockaddr.sin_zero);
            sockAddrs.push_back(sockaddr);
        }

        servSock = runSocket(servIp, port, servSockAddr, servSockAddrLen, SOCK_DGRAM);

        
        // connection between servers
        // shared resourses
    }

    Server::~Server()
    {
        close(servSock);
        exit(0);
    }

    string Server::sendMessageAndGetResponce(string ip, SubServerMessage message, string resourceName)
    {
        // std::ostream msg;

        return "";
    }

    int Server::runSocket(string servIp, int port, sockaddr_in &servSockAddr, unsigned int &servSockAddrLen, __socket_type sockType)
    {

        servSockAddr.sin_family = AF_INET;
        servSockAddr.sin_port = htons(port);
        servSockAddr.sin_addr.s_addr = inet_addr(servIp.c_str());
        memset(servSockAddr.sin_zero, '\0', sizeof servSockAddr.sin_zero);

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

                vector<string> lines = splitString("\n", str);
                vector<string> tokens;

                for (string s : lines)
                {
                    vector<string> split = splitString(" ", s);
                    tokens.insert(tokens.end(), split.begin(), split.end());
                }

                string method;
                string url;

                for (int i = 0; i < tokens.size(); i++)
                {
                    string s = tokens[i];
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
                cout << "METHOD AND URL" << method << url;
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

    void Server::sendResponse(int &connSock, const std::string &requestMethod, const std::string &requestURI, vector<string> tokens)
    {
        long bytesSent;
        std::ostringstream response;

        if (requestMethod == "GET")
            getFile(response, requestURI);
        else if (requestMethod == "POST")
        {

            int i = 0;

            for (int j = 0; j < tokens.size(); j++)
            {
                if (tokens[j] == "")
                {
                    if (j + 1 < tokens.size())
                        i = j + 1;
                    break;
                }
            }

            postFile(response, requestURI, tokens[i]);

            // Prepare the response

            // Handle POST request
            // Your logic for handling POST requests
        }
        else if (requestMethod == "DELETE")
        {
            // Handle DELETE request
            // Your logic for handling DELETE requests
        }
        else if (requestMethod == "UPDATE")
        {
            // Handle UPDATE request
            // Your logic for handling UPDATE requests
        }
        else
        {
            // Unsupported HTTP method
            response << "HTTP/1.1 405 Method Not Allowed\r\n"
                     << "Content-Length: 0\r\n"
                     << "Allow: GET, POST, DELETE, UPDATE\r\n"
                     << "Connection: close\r\n"
                     << "\r\n";
        }

        cout << "RESP" << endl;
        cout << "RESPONSE " << response.str() << endl;

        bytesSent = write(connSock, response.str().c_str(), response.str().size());
        if (bytesSent == response.str().size())
            log("Sent the response successfully");
        else
            log("Error sending response to client");
    }

    void Server::getFile(std::ostringstream &response, const std::string &requestURI)
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
            std::string errorMessage = "File not found: " + requestURI;
            response << "HTTP/1.1 404 Not Found\r\n"
                     << "Content-Type: text/plain\r\n"
                     << "Content-Length: " << errorMessage.size() << "\r\n"
                     << "Connection: close\r\n"
                     << "\r\n"
                     << errorMessage;
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

    void Server::formErrorResponse(int statusCode, const string &message, const string &statusText, std::ostringstream &response)
    {
        response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n"
                 << "Content-Type: text/plain\r\n"
                 << "Content-Length: " << message.size() << "\r\n"
                 << "Connection: close\r\n"
                 << "\r\n"
                 << message;
    }

}
