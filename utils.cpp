#include "utils.h"
#include <vector>
#include <string>

void log(const std::string &message)
{
    std::cout << message << std::endl;
}

void erroredExit(const std::string &errMessage)
{
    log("ERROR: " + errMessage);
    exit(1);
}

std::vector<std::string> splitString(std::string delimiter, std::string s)
{
    std::vector<std::string> res;

    size_t pos = 0;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {

        std::string token = s.substr(0, pos);
        res.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    if (s.size() != 0)
    {
        s.erase(s.size() - 1);
        res.push_back(s);
    }

    return res;
}