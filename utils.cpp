#include "utils.h"
void log(const string &message)
{
    cout << message << endl;
}

void erroredExit(const string &errMessage)
{
    log("ERROR: " + errMessage);
    exit(1);
}