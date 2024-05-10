#pragma once
#include <iostream>
#include <vector>

const int BUFFER_SIZE = 32768;

void log(const std::string &message);

void erroredExit(const std::string &errMessage);

std::vector<std::string> splitString(std::string delimiter, std::string s);
