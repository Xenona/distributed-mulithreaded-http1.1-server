#include "server.h"
#include "httpServer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "threadPool.h"
#include "nftables.h"

namespace
{
    const int BUFFER_SIZE = 32768;

    void log(const std::string &message)
    {
        std::cout << message << std::endl;
    }

    void erroredExit(const std::string &errMessage)
    {
        log("ERROR: " + errMessage);
        exit(1);
    }

}

std::vector<std::string> getServerIps(std::string filename, std::string myIp)
{
    std::ifstream ipFiles(filename);

    std::string ip;
    std::vector<std::string> ips;

    while (getline(ipFiles, ip))
    {
        if (ip.compare(myIp) != 0)
        {
            ips.push_back(ip);
        }
    }

    return ips;
}

int main(int argc, char *argv[])
{
    if (argc < 2) 
        erroredExit("The second argument must be your server's ip");

    std::string servIp = argv[1];

    std::vector<std::string> allyServers = getServerIps("serverIpList.txt", servIp);
    if (http::setLoadBalancer(allyServers))
        erroredExit("Couldn't set up nftables");

    http::ThreadPool *pool = new http::ThreadPool(4);
    http::Server server = http::Server(allyServers, servIp, 8081, pool);

    http::HttpServer httpserver = http::HttpServer(server, servIp, 8080);
    httpserver.startListen();

    return 0;
}