#include "tcpServer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "threadPool.h"

using namespace std;

vector<string> getServerIps(string filename, string myIp)
{
    ifstream ipFiles(filename);

    string ip;
    vector<string> ips;

    while (getline(ipFiles, ip))
    {
        if (ip.compare(myIp) != 0)
            ips.push_back(ip);
    }

    return ips;
}

void foo() {
    cout << "llalala" << endl;
}

int main()
{
    using namespace http;

    string servIp = "";

    cout << "Enter current subserver ip: " << endl;
    cin >> servIp;

    ThreadPool* pool = new ThreadPool(4);

    TcpServer server = TcpServer(getServerIps("serverIpList.txt", servIp), servIp, 8080, pool);
    server.startListen();

    return 0;
}