#include "../src/init.h"
#include "general.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <ctime>
#include <chrono>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <map>

auto Minecraft5 = std::make_unique<CMinecraft>();
using json = nlohmann::json;

std::string formatTimestamp(long long timestamp) {
    if (timestamp < 0) return "N/A";
    std::time_t timeInSeconds = timestamp / 1000;
    std::tm tm{};
    localtime_s(&tm, &timeInSeconds);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

std::optional<json> fetchJsonFromUrl(const std::string& url) {
    std::string response = performRequest(url);
    try {
        return json::parse(response);
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

int sumAllChallenges(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/player?key=" + getApiKey() + "&uuid=" + uuid;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) {
        return 0;
    }

    const auto& jsonResponse = jsonResponseOpt.value();
    if (!jsonResponse.contains("player")) {
        return 0;
    }

    const auto& playerData = jsonResponse["player"];
    if (!playerData.contains("challenges") || !playerData["challenges"].contains("all_time")) {
        return 0;
    }

    int totalChallenges = 0;
    const auto& allTimeChallenges = playerData["challenges"]["all_time"];
    for (const auto& [challengeName, value] : allTimeChallenges.items()) {
        totalChallenges += value.get<int>();
    }

    return totalChallenges;
}

std::map<std::string, int> getAchievementsPoints(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/resources/achievements?key=" + getApiKey() + "&uuid=" + uuid;
    std::map<std::string, int> achievementPoints;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) return achievementPoints;

    const auto& jsonResponse = jsonResponseOpt.value();
    if (jsonResponse.contains("achievements")) {
        auto achievements = jsonResponse["achievements"];
        for (const auto& [category, subcategoryMap] : achievements.items()) {
            if (subcategoryMap.contains("one_time")) {
                for (const auto& [id, achievementData] : subcategoryMap["one_time"].items()) {
                    int points = achievementData.value("points", 0);
                    std::string lowerId = toLowerCase(id);
                    std::string gameName = toLowerCase(category) + "_" + lowerId;
                    achievementPoints[gameName] = points;
                }
            }
        }
    }

    return achievementPoints;
}

std::map<std::string, std::vector<std::tuple<int, int, int>>> getTieredAchievements(const std::string& uuid) {
    const std::string url = "https://api.hypixel.net/resources/achievements?key=" + getApiKey();
    std::map<std::string, std::vector<std::tuple<int, int, int>>> tieredAchievements;

    auto jsonResponseOpt = fetchJsonFromUrl(url);
    if (!jsonResponseOpt.has_value()) return tieredAchievements;

    const auto& jsonResponse = jsonResponseOpt.value();
    if (jsonResponse.contains("achievements")) {
        auto achievements = jsonResponse["achievements"];
        for (const auto& [gameName, subcategory] : achievements.items()) {
            if (subcategory.contains("tiered")) {
                for (const auto& [achievementID, achievementData] : subcategory["tiered"].items()) {
                    std::vector<std::tuple<int, int, int>> tiersData;
                    for (const auto& tierData : achievementData["tiers"]) {
                        int tier = tierData.value("tier", 0);
                        int points = tierData.value("points", 0);
                        int amount = tierData.value("amount", 0);
                        tiersData.push_back({ tier, points, amount });
                    }
                    std::string formattedID = toLowerCase(gameName) + "_" + toLowerCase(achievementID);
                    tieredAchievements[formattedID] = tiersData;
                }
            }
        }
    }

    return tieredAchievements;
}

void getGeneralStats(const std::string& player) {
    const std::string& uuid = getUUID(player);

    if (uuid != "uuidfailed") {
        const std::string url = "https://api.hypixel.net/player?key=" + getApiKey() + "&uuid=" + uuid;

        std::map<std::string, int> tieredAchievements;
        std::vector<std::string> achievementsList;

        auto jsonResponseOpt = fetchJsonFromUrl(url);
        if (!jsonResponseOpt.has_value()) return;

        const auto& jsonResponse = jsonResponseOpt.value();
        if (!jsonResponse.contains("player")) return;

        const auto& playerData = jsonResponse["player"];

        long long firstLogin = playerData.value("firstLogin", -1ll);
        long long lastLogin = playerData.value("lastLogin", -1ll);
        int karma = playerData.value("karma", -1);
        double networkExp = playerData.value("networkExp", -1.0);
        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        if (playerData.contains("achievementsOneTime") && playerData["achievementsOneTime"].is_array()) {
            for (const auto& achievement : playerData["achievementsOneTime"]) {
                achievementsList.push_back(achievement.get<std::string>());
            }
        }

        if (playerData.contains("achievements")) {
            const auto& achievements = playerData["achievements"];
            for (const auto& [id, value] : achievements.items()) {
                if (value.is_number_integer()) {
                    tieredAchievements[id] = value.get<int>();
                }
            }
        }

        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;
        auto achievementPoints = getAchievementsPoints(uuid);

        int totalPoints = 0;
        for (const auto& achievement : achievementsList) {
            std::string lowerAchievement = toLowerCase(achievement);
            if (achievementPoints.find(lowerAchievement) != achievementPoints.end()) {
                totalPoints += achievementPoints[lowerAchievement];
            }
        }

        auto tieredData = getTieredAchievements(uuid);
        int totalTieredPoints = 0;

        for (const auto& [achievementID, tiers] : tieredData) {
            if (tieredAchievements.find(achievementID) != tieredAchievements.end()) {
                int playerValue = tieredAchievements[achievementID];
                int cumulativeTierPoints = 0;

                for (const auto& [tier, points, amount] : tiers) {
                    if (playerValue >= amount) {
                        cumulativeTierPoints += points;
                    }
                    else {
                        break;
                    }
                }
                totalTieredPoints += cumulativeTierPoints;
            }
        }

        totalPoints += totalTieredPoints;

        int totalChallenges = sumAllChallenges(uuid);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));
        ss << karma;
        std::string formatted_karma = ss.str();

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        if (networkLevel != -1) {

            Minecraft5->SendChatMessage("§e------------------------------------------------");
            Minecraft5->SendChatMessage("  §aGeneral stats for" + rankDisplay + " " + player + "                  ");
            Minecraft5->SendChatMessage("");
            Minecraft5->SendChatMessage("  §eNetwork level: §3" + std::to_string(networkLevel) + "                  ");
            Minecraft5->SendChatMessage("  §eTotal Achievement Points: §6" + std::to_string(totalPoints) + "                  ");
            Minecraft5->SendChatMessage("  §eTotal Challenges Completed: §6" + std::to_string(totalChallenges) + "                 ");
            Minecraft5->SendChatMessage("  §eKarma: §d" + formatted_karma + "                  ");
            Minecraft5->SendChatMessage("  §eFirst login: §a" + formatTimestamp(firstLogin) + "                  ");
            Minecraft5->SendChatMessage("  §eLast login: §a" + formatTimestamp(lastLogin) + "                  ");
            Minecraft5->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft5->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft5->SendChatMessage("§aPlayer does not exist          ");
    }
}
