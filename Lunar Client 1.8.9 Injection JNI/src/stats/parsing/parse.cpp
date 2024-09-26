#include "parse.h"
#include "../src/utils/http/Network.h"
#include "../src/utils/json/ExtractPlayer.h"
#include "../src/stats/qol/toLowerCase.h"

#include <iostream>

std::optional<json> fetchJsonFromUrl(const std::string& url) {
    std::cout << "Fetching JSON from URL: " << url << std::endl;
    std::string response = performRequest(url);
    try {
        auto jsonResponse = json::parse(response);
        return jsonResponse;
    }
    catch (const json::parse_error& e) {
        std::cout << "JSON parsing error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

int sumAllChallenges(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/player?key=" + getApiKey() + "&uuid=" + uuid;
    std::cout << "Summing all challenges for UUID: " << uuid << std::endl;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) {
        std::cout << "Failed to fetch JSON response for challenges." << std::endl;
        return 0;
    }

    const auto& jsonResponse = jsonResponseOpt.value();
    if (!jsonResponse.contains("player")) {
        std::cout << "No player data found in response for challenges." << std::endl;
        return 0;
    }

    const auto& playerData = jsonResponse["player"];
    if (!playerData.contains("challenges") || !playerData["challenges"].contains("all_time")) {
        std::cout << "No challenge data found for player." << std::endl;
        return 0;
    }

    int totalChallenges = 0;
    const auto& allTimeChallenges = playerData["challenges"]["all_time"];
    for (const auto& [challengeName, value] : allTimeChallenges.items()) {
        totalChallenges += value.get<int>();
        std::cout << "Challenge: " << challengeName << ", Value: " << value.get<int>() << std::endl;
    }

    std::cout << "Total Challenges: " << totalChallenges << std::endl;
    return totalChallenges;
}

std::map<std::string, std::pair<int, bool>> getAchievementsPoints(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/resources/achievements?key=" + getApiKey() + "&uuid=" + uuid;
    std::cout << "Fetching achievements points for UUID: " << uuid << std::endl;
    std::map<std::string, std::pair<int, bool>> achievementPoints;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) {
        std::cout << "Failed to fetch JSON response for achievements points." << std::endl;
        return achievementPoints;
    }

    const auto& jsonResponse = jsonResponseOpt.value();
    if (jsonResponse.contains("achievements")) {
        auto achievements = jsonResponse["achievements"];
        for (const auto& [category, subcategoryMap] : achievements.items()) {
            if (subcategoryMap.contains("one_time")) {
                for (const auto& [id, achievementData] : subcategoryMap["one_time"].items()) {
                    int points = achievementData.value("points", 0);
                    bool isLegacy = achievementData.value("legacy", false);
                    std::string lowerId = toLowerCase(id);
                    std::string gameName = toLowerCase(category) + "_" + lowerId;
                    achievementPoints[gameName] = { points, isLegacy };
                    std::cout << "Achievement: " << gameName << ", Points: " << points << ", Legacy: " << isLegacy << std::endl;
                }
            }
        }
    }
    else {
        std::cout << "No achievements found in the response." << std::endl;
    }

    return achievementPoints;
}

std::map<std::string, std::vector<std::tuple<int, int, int, bool>>> getTieredAchievements(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/resources/achievements?key=" + getApiKey();
    std::cout << "Fetching tiered achievements for UUID: " << uuid << std::endl;
    std::map<std::string, std::vector<std::tuple<int, int, int, bool>>> tieredAchievements;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) {
        std::cout << "Failed to fetch JSON response for tiered achievements." << std::endl;
        return tieredAchievements;
    }

    const auto& jsonResponse = jsonResponseOpt.value();
    if (jsonResponse.contains("achievements")) {
        auto achievements = jsonResponse["achievements"];
        for (const auto& [gameName, subcategory] : achievements.items()) {
            if (subcategory.contains("tiered")) {
                for (const auto& [achievementID, achievementData] : subcategory["tiered"].items()) {
                    std::vector<std::tuple<int, int, int, bool>> tiersData;
                    bool isLegacy = achievementData.value("legacy", false);
                    for (const auto& tierData : achievementData["tiers"]) {
                        int tier = tierData.value("tier", 0);
                        int points = tierData.value("points", 0);
                        int amount = tierData.value("amount", 0);
                        tiersData.push_back({ tier, points, amount, isLegacy });
                        std::cout << "Tiered Achievement: " << gameName << "_" << achievementID
                            << ", Tier: " << tier << ", Points: " << points << ", Amount: " << amount << ", Legacy: " << isLegacy << std::endl;
                    }
                    std::string formattedID = toLowerCase(gameName) + "_" + toLowerCase(achievementID);
                    tieredAchievements[formattedID] = tiersData;
                }
            }
        }
    }
    else {
        std::cout << "No tiered achievements found in the response." << std::endl;
    }

    return tieredAchievements;
}
