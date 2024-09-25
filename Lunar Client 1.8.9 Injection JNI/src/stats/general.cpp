#include "../src/init.h"
#include "../src/utils/json/ExtractRank.h"
#include "../src/stats/qol/formatTimestamp.h"
#include "../src/stats/qol/toLowerCase.h"
#include "../src/stats/parsing/parse.h"
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

void getGeneralStats(const std::string& player) {
    std::cout << "Fetching general stats for player: " << player << std::endl;

    const std::string& uuid = getUUID(player);
    std::cout << "UUID for player " << player << ": " << uuid << std::endl;

    if (uuid != "uuidfailed") {
        const std::string url = "https://api.hypixel.net/player?key=" + getApiKey() + "&uuid=" + uuid;
        std::cout << "API URL: " << url << std::endl;

        auto jsonResponseOpt = fetchJsonFromUrl(url);
        if (!jsonResponseOpt.has_value()) {
            std::cout << "Failed to fetch JSON response." << std::endl;
            return;
        }

        const auto& jsonResponse = jsonResponseOpt.value();
        if (!jsonResponse.contains("player")) {
            std::cout << "No player data found in response." << std::endl;
            return;
        }

        const auto& playerData = jsonResponse["player"];

        try {
            long long firstLogin = playerData.value("firstLogin", -1ll);
            long long lastLogin = playerData.value("lastLogin", -1ll);
            int karma = playerData.value("karma", -1);
            double networkExp = playerData.value("networkExp", -1.0);
            std::string newPackageRank = playerData.value("newPackageRank", "N/A");
            std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
            std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

            std::cout << "Player Data - First Login: " << firstLogin
                << ", Last Login: " << lastLogin
                << ", Karma: " << karma
                << ", Network Experience: " << networkExp
                << ", New Package Rank: " << newPackageRank
                << ", Monthly Package Rank: " << monthlyPackageRank
                << ", Rank Plus Color: " << rankPlusColor << std::endl;

            std::vector<std::string> achievementsList;
            if (playerData.contains("achievementsOneTime") && playerData["achievementsOneTime"].is_array()) {
                for (const auto& achievement : playerData["achievementsOneTime"]) {
                    if (achievement.is_string()) {
                        achievementsList.push_back(achievement.get<std::string>());
                    }
                }
            }
            else {
                std::cout << "No one-time achievements found or it's not an array." << std::endl;
            }

            std::cout << "One-time achievements: ";
            for (const auto& achievement : achievementsList) {
                std::cout << achievement << " ";
            }
            std::cout << std::endl;

            std::map<std::string, int> tieredAchievements;
            if (playerData.contains("achievements") && playerData["achievements"].is_object()) {
                const auto& achievements = playerData["achievements"];
                for (const auto& [id, value] : achievements.items()) {
                    if (value.is_number_integer()) {
                        tieredAchievements[id] = value.get<int>();
                    }
                    else {
                        std::cout << "Achievement ID " << id << " is not an integer." << std::endl;
                    }
                }
            }
            else {
                std::cout << "No tiered achievements found or it's not an object." << std::endl;
            }

            int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;
            auto achievementPoints = getAchievementsPoints(uuid);

            std::cout << "Network Level: " << networkLevel << std::endl;

            int totalPoints = 0;
            for (const auto& achievement : achievementsList) {
                std::string lowerAchievement = toLowerCase(achievement);
                if (achievementPoints.find(lowerAchievement) != achievementPoints.end()) {
                    totalPoints += achievementPoints[lowerAchievement];
                }
            }

            std::cout << "Total Achievement Points from one-time achievements: " << totalPoints << std::endl;

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
                    std::cout << "Achievement ID: " << achievementID << ", Player Value: " << playerValue
                        << ", Cumulative Tier Points: " << cumulativeTierPoints << std::endl;
                }
            }

            totalPoints += totalTieredPoints;
            std::cout << "Total Achievement Points after adding tiered points: " << totalPoints << std::endl;

            // Somme des challenges
            int totalChallenges = sumAllChallenges(uuid);
            std::cout << "Total Challenges Completed: " << totalChallenges << std::endl;

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
                Minecraft5->SendChatMessage(formatTimestamp(lastLogin) == "N/A" ? "  §eLast login: §aAPI OFF                  " : "  §eLast login: §a" + formatTimestamp(lastLogin) + "                  ");
                Minecraft5->SendChatMessage("§e------------------------------------------------");
            }
            else {
                Minecraft5->SendChatMessage("§aPlayer never joined Hypixel          ");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error processing player data: " << e.what() << std::endl;
        }
    }
    else {
        Minecraft5->SendChatMessage("§aPlayer does not exist          ");
    }
}
