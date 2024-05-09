#include "server.h"
#include "httpServer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
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

vector<string> getServerIps(string filename, string myIp)
{
    ifstream ipFiles(filename);

    string ip;
    vector<string> ips;

    while (getline(ipFiles, ip))
    {
        if (ip.compare(myIp) != 0)
        {
            ips.push_back(ip);
        }
    }

    return ips;
}

int main()
{
    using namespace http;

    string servIp = "";

    cout << "Enter current subserver ip: " << endl;
    cin >> servIp;

    ThreadPool *pool = new ThreadPool(4);

    Server server = Server(getServerIps("serverIpList.txt", servIp), servIp, 8081, pool);

    HttpServer httpserver = HttpServer(server, servIp, 8080);
    httpserver.startListen();

    return 0;
}