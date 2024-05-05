#include "tcpServer.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

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

int main()
{
    using namespace http;

    string servIp = "";

    cout << "Enter current subserver ip: " << endl;
    cin >> servIp;

    TcpServer server = TcpServer(getServerIps("serverIpList.txt", servIp), servIp, 8080);
    server.startListen();

    return 0;
}