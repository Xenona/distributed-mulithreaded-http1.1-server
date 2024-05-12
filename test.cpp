// #include <sys/socket.h>
// #include <unistd.h>
// #include "nftables.h"
// #include <arpa/inet.h>
// #include <string>
// #include <cstring>

// int main()
// {

//     std::string servIp = "192.168.114.1";
//     sockaddr_in servsockaddr;
//     servsockaddr.sin_family = AF_INET;
//     servsockaddr.sin_port = htons(8080);
//     servsockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
//     memset(servsockaddr.sin_zero, '\0', sizeof servsockaddr.sin_zero);


//     std::string ip = "192.168.114.1";
//     sockaddr_in sockaddr;
//     sockaddr.sin_family = AF_INET;
//     sockaddr.sin_port = htons(8080);
//     sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
//     memset(sockaddr.sin_zero, '\0', sizeof sockaddr.sin_zero);


//     int servSock = socket(AF_INET, SOCK_STREAM, 0);
//     if (servSock < 0)
//         exit(1);

//     const int enable = 1;
//     if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
//         exit(2);

//     if (setsockopt(servSock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
//         exit(3);

//     if (bind(servSock, (struct sockaddr *)&servsockaddr, sizeof(servsockaddr)) < 0)
//         exit(4);


//     int res = connect(servSock, (struct sockaddr *)&servsockaddr, sizeof(servsockaddr));
//     if (res)
//     {
//         std::cout << "AAAAAAAAAAAAAAAAAAa" << std::endl;
//     }

//     return 0;
// }