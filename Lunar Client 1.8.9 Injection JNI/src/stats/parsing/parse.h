#pragma once

#include <optional>
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::optional<json> fetchJsonFromUrl(const std::string& url);
int sumAllChallenges(const std::string& uuid);
std::map<std::string, std::pair<int, bool>> getAchievementsPoints(const std::string& uuid);
std::map<std::string, std::vector<std::tuple<int, int, int, bool>>> getTieredAchievements(const std::string& uuid);
