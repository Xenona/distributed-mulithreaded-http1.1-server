#include <iostream>
#include <nftables/libnftables.h>
#include <string>
#include <vector>

namespace http {
    int setLoadBalancer(std::vector<std::string> ips);
}