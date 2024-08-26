#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <fstream>
#include <random>
#include <regex>
#include <algorithm>
#include <thread>

#include "init.h"

void readLogFile(const std::string& filePath);
std::string generateRandomCaseMessage(const std::string& message);
int countColons(const std::string& line);


