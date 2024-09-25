#pragma once
#include <string>

#include "../src/SDK/Minecraft.h"

std::string rankUtils(std::string response);
std::string getRankDisplay(const std::string& newPackageRank, const std::string& monthlyPackageRank, const std::string& rankPlusColor);
