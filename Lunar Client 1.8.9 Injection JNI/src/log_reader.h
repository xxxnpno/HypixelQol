#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <random>
#include <regex>
#include <algorithm>
#include <thread>
#include <deque>
#include <vector>

#include "init.h"
#include "../src/stats/link.h"

void readLogFile(const std::string& filePath);
std::string generateRandomCaseMessage(const std::string& message);
int countColons(const std::string& line);


