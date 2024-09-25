#include "ExtractRank.h"
#include "ExtractPlayer.h"
#include "../src/utils/http/Network.h"

#include <nlohmann/json.hpp>

auto Minecraft8347567834 = std::make_unique<CMinecraft>();

using json = nlohmann::json;

std::string newPackageRank;
std::string monthlyPackageRank;
std::string rankPlusColor;

void extractRanks(const std::string& response) {
    try {
        auto jsonResponse = json::parse(response);
        if (jsonResponse.contains("player")) {
            if (jsonResponse["player"].contains("newPackageRank")) {
                newPackageRank = jsonResponse["player"]["newPackageRank"];
            }
            else {
                newPackageRank = "N/A";
            }

            if (jsonResponse["player"].contains("monthlyPackageRank")) {
                monthlyPackageRank = jsonResponse["player"]["monthlyPackageRank"];
            }
            else {
                monthlyPackageRank = "N/A";
            }

            if (jsonResponse["player"].contains("rankPlusColor")) {
                rankPlusColor = jsonResponse["player"]["rankPlusColor"];
            }
            else {
                rankPlusColor = "N/A";
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        newPackageRank = "error";
        monthlyPackageRank = "error";
        rankPlusColor = "error";
    }
}

const std::unordered_map<std::string, std::string> newPackageRankMap = {
    {"VIP", " §a[VIP]"},
    {"VIP_PLUS", " §a[VIP§e+§a]"},
    {"MVP", " §b[MVP]"},
    {"MVP_PLUS", " §b[MVP§c+§b]"}
};

const std::unordered_map<std::string, std::string> rankPlusColorMap = {
    {"BLACK", " §b[MVP§0+§b]"},
    {"DARK_BLUE", " §b[MVP§1+§b]"},
    {"DARK_GRAY", " §b[MVP§8+§b]"},
    {"DARK_PURPLE", " §b[MVP§5+§b]"},
    {"DARK_AQUA", " §b[MVP§3+§b]"},
    {"DARK_RED", " §b[MVP§4+§b]"},
    {"DARK_GREEN", " §b[MVP§2+§b]"},
    {"BLUE", " §b[MVP§9+§b]"},
    {"WHITE", " §b[MVP§f+§b]"},
    {"LIGHT_PURPLE", " §b[MVP§d+§b]"},
    {"YELLOW", " §b[MVP§e+§b]"},
    {"GREEN", " §b[MVP§a+§b]"},
    {"GOLD", " §b[MVP§6+§b]"}
};

const std::unordered_map<std::string, std::string> monthlyPackageRankMap = {
    {"SUPERSTAR", " §6[MVP§c++§6]"}
};

const std::unordered_map<std::string, std::string> superstarPlusColorMap = {
    {"BLACK", " §6[MVP§0++§6]"},
    {"DARK_BLUE", " §6[MVP§1++§6]"},
    {"DARK_GRAY", " §6[MVP§8++§6]"},
    {"DARK_PURPLE", " §6[MVP§5++§6]"},
    {"DARK_AQUA", " §6[MVP§3++§6]"},
    {"DARK_RED", " §6[MVP§4++§6]"},
    {"DARK_GREEN", " §6[MVP§2++§6]"},
    {"BLUE", " §6[MVP§9++§6]"},
    {"WHITE", " §6[MVP§f++§6]"},
    {"LIGHT_PURPLE", " §6[MVP§d++§6]"},
    {"YELLOW", " §6[MVP§e++§6]"},
    {"GREEN", " §6[MVP§a++§6]"},
    {"GOLD", " §6[MVP++§6]"}
};

std::string getRankDisplay(const std::string& newPackageRank, const std::string& monthlyPackageRank, const std::string& rankPlusColor) {
    std::string rankDisplay = "§7";

    if (newPackageRankMap.find(newPackageRank) != newPackageRankMap.end()) {
        rankDisplay = newPackageRankMap.at(newPackageRank);
        if (newPackageRank == "MVP_PLUS" && rankPlusColorMap.find(rankPlusColor) != rankPlusColorMap.end()) {
            rankDisplay = rankPlusColorMap.at(rankPlusColor);
        }
    }

    if (monthlyPackageRank == "SUPERSTAR") {
        rankDisplay = monthlyPackageRankMap.at("SUPERSTAR");
        if (superstarPlusColorMap.find(rankPlusColor) != superstarPlusColorMap.end()) {
            rankDisplay = superstarPlusColorMap.at(rankPlusColor);
        }
    }
    return rankDisplay;
}

std::string rankUtils(std::string response) {

	extractRanks(performRequest(response));

	return getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);
}