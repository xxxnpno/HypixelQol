#pragma once
#include "../log_reader.h"
#include "../SDK/Minecraft.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include <Windows.h>
#include <shlobj.h>

void init(void* instance);
std::string getApiKey();
std::string getUserPath();
std::string performRequest(const std::string& url);
std::string getUUID(const std::string& pseudo);
void extractRanks(const std::string& response);
std::string getRankDisplay(const std::string& newPackageRank, const std::string& monthlyPackageRank, const std::string& rankPlusColor);
const std::unordered_map<std::string, std::string> getrankPlusColorMap();
const std::unordered_map<std::string, std::string> getmonthlyPackageRankMap();
const std::unordered_map<std::string, std::string> getSuperstarPlusColorMap();
const std::unordered_map<std::string, std::string> getnewPackageRankMap();


