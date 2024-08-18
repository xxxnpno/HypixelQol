#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <fstream>
#include <regex>

#include "init.h"

#include <json/json.h>  

void readLogFile(const std::string& filePath);
void handleLogLine(const std::string& line);
