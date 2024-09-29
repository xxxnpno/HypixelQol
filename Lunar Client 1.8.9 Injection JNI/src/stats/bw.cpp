#include "../src/init.h"
#include "../src/stats/parsing/parse.h"
#include "bw.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <optional>
#include <iomanip>
#include <map>
#include <vector>
#include <functional>

auto Minecraft3 = std::make_unique<CMinecraft>();
using json = nlohmann::json;

std::string getBedwarsRationColor(double ratio, const std::string& type) {
    if (type == "WLR") {
        if (ratio >= 25.00) return "§0";
        if (ratio >= 8.00) return "§4";
        if (ratio >= 4.00) return "§c";
        if (ratio >= 3.00) return "§e";
        if (ratio >= 2.00) return "§2";
        if (ratio >= 1.00) return "§a"; 
        if (ratio >= 0.00) return "§f"; 
        return "§7"; 
    }
    else if (type == "FKDR") {
        if (ratio >= 200.00) return "§0";
        if (ratio >= 50.00) return "§4";
        if (ratio >= 20.00) return "§c";
        if (ratio >= 10.00) return "§e";
        if (ratio >= 4.00) return "§2";
        if (ratio >= 2.00) return "§a";
        if (ratio >= 1.00) return "§f"; 
        return "§7"; 
    }
    else if (type == "KDR") {
        if (ratio >= 5.00) return "§0";
        if (ratio >= 2.00) return "§4";
        if (ratio >= 1.00) return "§e"; 
        if (ratio >= 0.00) return "§f"; 
        return "§7"; 
    }
    return "§7"; 
}

std::string getBedwarsPrestigeColor(int level) {
    static const std::map<int, std::string> prestigeColors = {
        {100, "§7"},
        {200, "§f"},
        {300, "§6"},
        {400, "§b"},
        {500, "§2"},
        {600, "§3"},
        {700, "§4"},
        {800, "§5"},
        {900, "§1"},
        {1000, "§d"}
    };

    static const std::vector<std::pair<int, std::function<std::string(int)>>> specialPrestigeRanges = {
        {1000, [](int lvl) { return "§c[§6" + std::to_string((lvl / 1000) % 10) + "§e" + std::to_string((lvl / 100) % 10) +
                                      "§a" + std::to_string((lvl / 10) % 10) + "§b" + std::to_string(lvl % 10) + "§k#§r§d]"; }},
        {1100, [](int lvl) { return "§8[§7" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§7" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1200, [](int lvl) { return "§8[§e" + std::to_string((lvl / 1000) % 10) + "§e" + std::to_string((lvl / 100) % 10) +
                                      "§e" + std::to_string((lvl / 10) % 10) + "§e" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1300, [](int lvl) { return "§8[§b" + std::to_string((lvl / 1000) % 10) + "§b" + std::to_string((lvl / 100) % 10) +
                                      "§b" + std::to_string((lvl / 10) % 10) + "§b" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1400, [](int lvl) { return "§8[§a" + std::to_string((lvl / 1000) % 10) + "§a" + std::to_string((lvl / 100) % 10) +
                                      "§a" + std::to_string((lvl / 10) % 10) + "§a" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1500, [](int lvl) { return "§8[§3" + std::to_string((lvl / 1000) % 10) + "§3" + std::to_string((lvl / 100) % 10) +
                                      "§3" + std::to_string((lvl / 10) % 10) + "§3" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1600, [](int lvl) { return "§8[§c" + std::to_string((lvl / 1000) % 10) + "§c" + std::to_string((lvl / 100) % 10) +
                                      "§c" + std::to_string((lvl / 10) % 10) + "§c" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1700, [](int lvl) { return "§8[§5" + std::to_string((lvl / 1000) % 10) + "§5" + std::to_string((lvl / 100) % 10) +
                                      "§5" + std::to_string((lvl / 10) % 10) + "§5" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1800, [](int lvl) { return "§8[§1" + std::to_string((lvl / 1000) % 10) + "§1" + std::to_string((lvl / 100) % 10) +
                                      "§1" + std::to_string((lvl / 10) % 10) + "§1" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {1900, [](int lvl) { return "§8[§d" + std::to_string((lvl / 1000) % 10) + "§d" + std::to_string((lvl / 100) % 10) +
                                      "§d" + std::to_string((lvl / 10) % 10) + "§d" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {2000, [](int lvl) { return "§0[§8" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§8" + std::to_string(lvl % 10) + "§k#§r§0]"; }},
        {2100, [](int lvl) { return "§7[§7" + std::to_string((lvl / 1000) % 10) + "§e" + std::to_string((lvl / 100) % 10) +
                                      "§e" + std::to_string((lvl / 10) % 10) + "§6" + std::to_string(lvl % 10) + "§k#§r§6]"; }},
        {2200, [](int lvl) { return "§6[§6" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§3" + std::to_string(lvl % 10) + "§k#§r§3]"; }},
        {2300, [](int lvl) { return "§5[§5" + std::to_string((lvl / 1000) % 10) + "§d" + std::to_string((lvl / 100) % 10) +
                                      "§d" + std::to_string((lvl / 10) % 10) + "§6" + std::to_string(lvl % 10) + "§k#§r§e]"; }},
        {2400, [](int lvl) { return "§b[§b" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§8" + std::to_string(lvl % 10) + "§k#§r§0]"; }},
        {2500, [](int lvl) { return "§7[§7" + std::to_string((lvl / 1000) % 10) + "§a" + std::to_string((lvl / 100) % 10) +
                                      "§a" + std::to_string((lvl / 10) % 10) + "§2" + std::to_string(lvl % 10) + "§k#§r§2]"; }},
        {2600, [](int lvl) { return "§4[§4" + std::to_string((lvl / 1000) % 10) + "§c" + std::to_string((lvl / 100) % 10) +
                                      "§c" + std::to_string((lvl / 10) % 10) + "§d" + std::to_string(lvl % 10) + "§k#§r§5]"; }},
        {2700, [](int lvl) { return "§e[§e" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§0" + std::to_string(lvl % 10) + "§k#§r§0]"; }},
        {2800, [](int lvl) { return "§a[§a" + std::to_string((lvl / 1000) % 10) + "§2" + std::to_string((lvl / 100) % 10) +
                                      "§2" + std::to_string((lvl / 10) % 10) + "§7" + std::to_string(lvl % 10) + "§k#§r§e]"; }},
        {2900, [](int lvl) { return "§b[§b" + std::to_string((lvl / 1000) % 10) + "§3" + std::to_string((lvl / 100) % 10) +
                                      "§3" + std::to_string((lvl / 10) % 10) + "§1" + std::to_string(lvl % 10) + "§k#§r§1]"; }},
        {3000, [](int lvl) { return "§e[§e" + std::to_string((lvl / 1000) % 10) + "§7" + std::to_string((lvl / 100) % 10) +
                                      "§7" + std::to_string((lvl / 10) % 10) + "§c" + std::to_string(lvl % 10) + "§k#§r§4]"; }},
        // -> 5000 +
    };

    for (const auto& [minLevel, color] : prestigeColors) {
        if (level < minLevel) {
            return color + " [" + std::to_string(level) + "§k#§r" + color + "]";
        }
    }

    for (const auto& [minLevel, formatFunc] : specialPrestigeRanges) {
        if (level >= minLevel && level < minLevel + 100) {
            return formatFunc(level);
        }
    }
    return " §7[" + std::to_string(level) + "§k#§r§7]";
}

void getBwStats(const std::string& player) {
    std::cout << "Fetching Bedwars stats for player: " << player << std::endl;

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

        int bedwarsWins = 0;
        int bedwarsLosses = 0;
        int finalKills = 0;
        int finalDeaths = 0;
        int kills = 0;
        int deaths = 0;
        
        if (playerData.contains("stats") && playerData["stats"].contains("Bedwars")) {
            const auto& bedwarsStats = playerData["stats"]["Bedwars"];
            bedwarsWins = bedwarsStats.value("wins_bedwars", 0);
            bedwarsLosses = bedwarsStats.value("losses_bedwars", 0);
            finalKills = bedwarsStats.value("final_kills_bedwars", 0);
            finalDeaths = bedwarsStats.value("final_deaths_bedwars", 0);
            kills = bedwarsStats.value("kills_bedwars", 0);
            deaths = bedwarsStats.value("deaths_bedwars", 0);
        }
        else {
            std::cout << "No Bedwars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int level = playerData.value("achievements", json::object()).value("bedwars_level", 0);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        double FKDR = (finalDeaths > 0) ? finalKills / static_cast<double>(finalDeaths) : static_cast<double>(finalKills);
        double WLR = (bedwarsLosses > 0) ? bedwarsWins / static_cast<double>(bedwarsLosses) : static_cast<double>(bedwarsWins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << bedwarsWins;
        std::string formatted_bedwarsWins = ss.str();
        ss.str("");

        ss << finalKills;
        std::string formatted_finalKills = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << FKDR;
        std::string formatted_FKDR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string formatte_level = getBedwarsPrestigeColor(level);

        std::string colored_WLR = getBedwarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_FKDR = getBedwarsRationColor(FKDR, "FKDR") + formatted_FKDR;
        std::string colored_KDR = getBedwarsRationColor(KDR, "KDR") + formatted_KDR;

        if (networkLevel != -1) {
            Minecraft3->SendChatMessage("§e------------------------------------------------");
            Minecraft3->SendChatMessage("  §aBedwars stats for " + formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft3->SendChatMessage("");
            Minecraft3->SendChatMessage("  §eWins: §6" + formatted_bedwarsWins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft3->SendChatMessage("  §eFinal Kills: §6" + formatted_finalKills + "§e - FKDR: " + colored_FKDR + "                  ");
            Minecraft3->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft3->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft3->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft3->SendChatMessage("§aPlayer does not exist.");
    }
}

void getSoloBwStats(const std::string& player) {
    std::cout << "Fetching Bedwars stats for player: " << player << std::endl;

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

        int bedwarsWins = 0;
        int bedwarsLosses = 0;
        int finalKills = 0;
        int finalDeaths = 0;
        int kills = 0;
        int deaths = 0;

        if (playerData.contains("stats") && playerData["stats"].contains("Bedwars")) {
            const auto& bedwarsStats = playerData["stats"]["Bedwars"];
            bedwarsWins = bedwarsStats.value("eight_one_wins_bedwars", 0);
            bedwarsLosses = bedwarsStats.value("eight_one_losses_bedwars", 0);
            finalKills = bedwarsStats.value("eight_one_final_kills_bedwars", 0);
            finalDeaths = bedwarsStats.value("eight_one_final_deaths_bedwars", 0);
            kills = bedwarsStats.value("eight_one_kills_bedwars", 0);
            deaths = bedwarsStats.value("eight_one_deaths_bedwars", 0);
        }
        else {
            std::cout << "No Bedwars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int level = playerData.value("achievements", json::object()).value("bedwars_level", 0);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        double FKDR = (finalDeaths > 0) ? finalKills / static_cast<double>(finalDeaths) : static_cast<double>(finalKills);
        double WLR = (bedwarsLosses > 0) ? bedwarsWins / static_cast<double>(bedwarsLosses) : static_cast<double>(bedwarsWins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << bedwarsWins;
        std::string formatted_bedwarsWins = ss.str();
        ss.str("");

        ss << finalKills;
        std::string formatted_finalKills = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << FKDR;
        std::string formatted_FKDR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string formatte_level = getBedwarsPrestigeColor(level);

        std::string colored_WLR = getBedwarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_FKDR = getBedwarsRationColor(FKDR, "FKDR") + formatted_FKDR;
        std::string colored_KDR = getBedwarsRationColor(KDR, "KDR") + formatted_KDR;

        if (networkLevel != -1) {
            Minecraft3->SendChatMessage("§e------------------------------------------------");
            Minecraft3->SendChatMessage("  §aSolo Bedwars stats for " + formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft3->SendChatMessage("");
            Minecraft3->SendChatMessage("  §eWins: §6" + formatted_bedwarsWins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft3->SendChatMessage("  §eFinal Kills: §6" + formatted_finalKills + "§e - FKDR: " + colored_FKDR + "                  ");
            Minecraft3->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft3->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft3->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft3->SendChatMessage("§aPlayer does not exist.");
    }
}

void getDuoBwStats(const std::string& player) {
    std::cout << "Fetching Bedwars stats for player: " << player << std::endl;

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

        int bedwarsWins = 0;
        int bedwarsLosses = 0;
        int finalKills = 0;
        int finalDeaths = 0;
        int kills = 0;
        int deaths = 0;

        if (playerData.contains("stats") && playerData["stats"].contains("Bedwars")) {
            const auto& bedwarsStats = playerData["stats"]["Bedwars"];
            bedwarsWins = bedwarsStats.value("eight_two_wins_bedwars", 0);
            bedwarsLosses = bedwarsStats.value("eight_two_losses_bedwars", 0);
            finalKills = bedwarsStats.value("eight_two_final_kills_bedwars", 0);
            finalDeaths = bedwarsStats.value("eight_two_final_deaths_bedwars", 0);
            kills = bedwarsStats.value("eight_two_kills_bedwars", 0);
            deaths = bedwarsStats.value("eight_two_deaths_bedwars", 0);
        }
        else {
            std::cout << "No Bedwars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int level = playerData.value("achievements", json::object()).value("bedwars_level", 0);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        double FKDR = (finalDeaths > 0) ? finalKills / static_cast<double>(finalDeaths) : static_cast<double>(finalKills);
        double WLR = (bedwarsLosses > 0) ? bedwarsWins / static_cast<double>(bedwarsLosses) : static_cast<double>(bedwarsWins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << bedwarsWins;
        std::string formatted_bedwarsWins = ss.str();
        ss.str("");

        ss << finalKills;
        std::string formatted_finalKills = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << FKDR;
        std::string formatted_FKDR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string formatte_level = getBedwarsPrestigeColor(level);

        std::string colored_WLR = getBedwarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_FKDR = getBedwarsRationColor(FKDR, "FKDR") + formatted_FKDR;
        std::string colored_KDR = getBedwarsRationColor(KDR, "KDR") + formatted_KDR;

        if (networkLevel != -1) {
            Minecraft3->SendChatMessage("§e------------------------------------------------");
            Minecraft3->SendChatMessage("  §aDuo Bedwars stats for " + formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft3->SendChatMessage("");
            Minecraft3->SendChatMessage("  §eWins: §6" + formatted_bedwarsWins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft3->SendChatMessage("  §eFinal Kills: §6" + formatted_finalKills + "§e - FKDR: " + colored_FKDR + "                  ");
            Minecraft3->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft3->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft3->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft3->SendChatMessage("§aPlayer does not exist.");
    }
}

void getTrioBwStats(const std::string& player) {
    std::cout << "Fetching Bedwars stats for player: " << player << std::endl;

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

        int bedwarsWins = 0;
        int bedwarsLosses = 0;
        int finalKills = 0;
        int finalDeaths = 0;
        int kills = 0;
        int deaths = 0;

        if (playerData.contains("stats") && playerData["stats"].contains("Bedwars")) {
            const auto& bedwarsStats = playerData["stats"]["Bedwars"];
            bedwarsWins = bedwarsStats.value("four_three_wins_bedwars", 0);
            bedwarsLosses = bedwarsStats.value("four_three_losses_bedwars", 0);
            finalKills = bedwarsStats.value("four_three_final_kills_bedwars", 0);
            finalDeaths = bedwarsStats.value("four_three_final_deaths_bedwars", 0);
            kills = bedwarsStats.value("four_three_kills_bedwars", 0);
            deaths = bedwarsStats.value("four_three_deaths_bedwars", 0);
        }
        else {
            std::cout << "No Bedwars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int level = playerData.value("achievements", json::object()).value("bedwars_level", 0);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        double FKDR = (finalDeaths > 0) ? finalKills / static_cast<double>(finalDeaths) : static_cast<double>(finalKills);
        double WLR = (bedwarsLosses > 0) ? bedwarsWins / static_cast<double>(bedwarsLosses) : static_cast<double>(bedwarsWins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << bedwarsWins;
        std::string formatted_bedwarsWins = ss.str();
        ss.str("");

        ss << finalKills;
        std::string formatted_finalKills = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << FKDR;
        std::string formatted_FKDR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string formatte_level = getBedwarsPrestigeColor(level);

        std::string colored_WLR = getBedwarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_FKDR = getBedwarsRationColor(FKDR, "FKDR") + formatted_FKDR;
        std::string colored_KDR = getBedwarsRationColor(KDR, "KDR") + formatted_KDR;

        if (networkLevel != -1) {
            Minecraft3->SendChatMessage("§e------------------------------------------------");
            Minecraft3->SendChatMessage("  §a3v3v3v3 Bedwars stats for " + formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft3->SendChatMessage("");
            Minecraft3->SendChatMessage("  §eWins: §6" + formatted_bedwarsWins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft3->SendChatMessage("  §eFinal Kills: §6" + formatted_finalKills + "§e - FKDR: " + colored_FKDR + "                  ");
            Minecraft3->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft3->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft3->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft3->SendChatMessage("§aPlayer does not exist.");
    }
}

void getQuatuorBwStats(const std::string& player) {
    std::cout << "Fetching Bedwars stats for player: " << player << std::endl;

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

        int bedwarsWins = 0;
        int bedwarsLosses = 0;
        int finalKills = 0;
        int finalDeaths = 0;
        int kills = 0;
        int deaths = 0;

        if (playerData.contains("stats") && playerData["stats"].contains("Bedwars")) {
            const auto& bedwarsStats = playerData["stats"]["Bedwars"];
            bedwarsWins = bedwarsStats.value("four_four_wins_bedwars", 0);
            bedwarsLosses = bedwarsStats.value("four_four_losses_bedwars", 0);
            finalKills = bedwarsStats.value("four_four_final_kills_bedwars", 0);
            finalDeaths = bedwarsStats.value("four_four_final_deaths_bedwars", 0);
            kills = bedwarsStats.value("four_four_kills_bedwars", 0);
            deaths = bedwarsStats.value("four_four_deaths_bedwars", 0);
        }
        else {
            std::cout << "No Bedwars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int level = playerData.value("achievements", json::object()).value("bedwars_level", 0);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        double FKDR = (finalDeaths > 0) ? finalKills / static_cast<double>(finalDeaths) : static_cast<double>(finalKills);
        double WLR = (bedwarsLosses > 0) ? bedwarsWins / static_cast<double>(bedwarsLosses) : static_cast<double>(bedwarsWins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << bedwarsWins;
        std::string formatted_bedwarsWins = ss.str();
        ss.str("");

        ss << finalKills;
        std::string formatted_finalKills = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << FKDR;
        std::string formatted_FKDR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string formatte_level = getBedwarsPrestigeColor(level);

        std::string colored_WLR = getBedwarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_FKDR = getBedwarsRationColor(FKDR, "FKDR") + formatted_FKDR;
        std::string colored_KDR = getBedwarsRationColor(KDR, "KDR") + formatted_KDR;

        if (networkLevel != -1) {
            Minecraft3->SendChatMessage("§e------------------------------------------------");
            Minecraft3->SendChatMessage("  §a4v4v4v4 Bedwars stats for " + formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft3->SendChatMessage("");
            Minecraft3->SendChatMessage("  §eWins: §6" + formatted_bedwarsWins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft3->SendChatMessage("  §eFinal Kills: §6" + formatted_finalKills + "§e - FKDR: " + colored_FKDR + "                  ");
            Minecraft3->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft3->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft3->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft3->SendChatMessage("§aPlayer does not exist.");
    }
}
