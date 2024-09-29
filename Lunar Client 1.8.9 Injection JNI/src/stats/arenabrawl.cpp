#include "../src/init.h"
#include "../src/stats/parsing/parse.h"
#include "sw.h"

auto Minecraft911 = std::make_unique<CMinecraft>();

std::string getArenaPrestigeColor(int wins) {
    if (wins >= 15000) {
        return "§c[§6" + std::to_string((wins / 10000) % 10) + "§e" + std::to_string((wins / 1000) % 10) + "§b" + std::to_string((wins / 100) % 10) + "§d" + std::to_string((wins / 10) % 10) + "§9" + std::to_string(wins % 10) + "§c]";
    }

    static const std::map<int, std::string> prestigeColors = {
        {500, "§f"},
        {1000, "§a"},
        {2000, "§2"},
        {3000, "§d"},
        {4000, "§5"},
        {5000, "§c"},
        {7500, "§4"},
        {10000, "§6"},
    };

    for (auto it = prestigeColors.rbegin(); it != prestigeColors.rend(); ++it) {
        if (wins >= it->first) {
            return it->second + "[" + std::to_string(wins) + "]";
        }
    }

    return "§7[" + std::to_string(wins) + "]";
}

std::string getArenaBrawlRationColor(double ratio, const std::string& type) {
    if (type == "WLR") {
        if (ratio >= 25.00) return "§0";
        if (ratio >= 10.00) return "§4";
        if (ratio >= 6.00) return "§c";
        if (ratio >= 3.00) return "§e";
        if (ratio >= 1.50) return "§2";
        if (ratio >= 0.70) return "§a";
        if (ratio >= 0.30) return "§f";
        if (ratio >= 0.00) return "§7";
        return "§7";
    }
    else if (type == "KDR") {
        if (ratio >= 25.00) return "§0";
        if (ratio >= 10.00) return "§4";
        if (ratio >= 6.00) return "§c";
        if (ratio >= 3.00) return "§e";
        if (ratio >= 1.50) return "§2";
        if (ratio >= 0.70) return "§a";
        if (ratio >= 0.30) return "§f";
        if (ratio >= 0.00) return "§7";
        return "§7";
    }
    return "§7";
}

void getArenaBrawlStats(const std::string& player) {

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
        
        int wins = 0;
        int losses1v1 = 0, deaths1v1 = 0, kills1v1 = 0;
        int losses2v2 = 0, deaths2v2 = 0, kills2v2 = 0;
        int losses4v4 = 0, deaths4v4 = 0, kills4v4 = 0;
        std::string offensive, support, utility, ultimate = "N/A";
        std::string rune = "N/A";
        std::string prefixColor = "N/A";
     
        if (playerData.contains("stats") && playerData["stats"].contains("Arena")) {
            const auto& arenaBrawlStats = playerData["stats"]["Arena"];
            wins = arenaBrawlStats.value("wins", 0);
            losses1v1 = arenaBrawlStats.value("losses_1v1", 0);
            losses2v2 = arenaBrawlStats.value("losses_2v2", 0);
            losses4v4 = arenaBrawlStats.value("losses_4v4", 0);
            deaths1v1 = arenaBrawlStats.value("deaths_1v1", 0);
            deaths2v2 = arenaBrawlStats.value("deaths_2v2", 0);
            deaths4v4 = arenaBrawlStats.value("deaths_4v4", 0);
            kills1v1 = arenaBrawlStats.value("kills_1v1", 0);
            kills2v2 = arenaBrawlStats.value("kills_2v2", 0);
            kills4v4 = arenaBrawlStats.value("kills4v4", 0);
            offensive = arenaBrawlStats.value("offensive", "N/A");
            support = arenaBrawlStats.value("support", "N/A");
            utility = arenaBrawlStats.value("utility", "N/A");
            ultimate = arenaBrawlStats.value("ultimate", "N/A");
            rune = arenaBrawlStats.value("active_rune", "N/A");
            prefixColor = arenaBrawlStats.value("prefix_color", "N/A");
        }
        else {
            std::cout << "No Arena stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        int losses = losses1v1 + losses2v2 + losses4v4;
        int deaths = deaths1v1 + deaths2v2 + deaths4v4;
        int kills = kills1v1 + kills2v2 + kills4v4;

        double WLR = (losses > 0) ? wins / static_cast<double>(losses) : static_cast<double>(wins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << wins;
        std::string formatted_wins = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string colored_WLR = getArenaBrawlRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_KDR = getArenaBrawlRationColor(KDR, "KDR") + formatted_KDR;

        std::string formatte_wins = getArenaPrestigeColor(wins);


        if (networkLevel != -1) {
            Minecraft911->SendChatMessage("§e------------------------------------------------");
            Minecraft911->SendChatMessage("  §aArena Brawl stats for " + formatte_wins +  rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft911->SendChatMessage("");
            Minecraft911->SendChatMessage("  §eOffensive: §c" + offensive + " §eUtility: §6" + utility + " §eSupport: §a" + support + " §eUltimate: §6" + ultimate + " §eRune: §4" + rune + "                  ");
            Minecraft911->SendChatMessage("  §eWins: §6" + formatted_wins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft911->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft911->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft911->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft911->SendChatMessage("§aPlayer does not exist         ");
    }
}

void get1v1ArenaBrawlStats(const std::string& player) {

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

        int wins = 0, overallWins = 0;
        int losses1v1 = 0, deaths1v1 = 0, kills1v1 = 0;

        std::string offensive, support, utility, ultimate = "N/A";
        std::string rune = "N/A";
        std::string prefixColor = "N/A";

        if (playerData.contains("stats") && playerData["stats"].contains("Arena")) {
            const auto& arenaBrawlStats = playerData["stats"]["Arena"];
            overallWins = arenaBrawlStats.value("wins", 0);
            wins = arenaBrawlStats.value("wins_1v1", 0);
            losses1v1 = arenaBrawlStats.value("losses_1v1", 0);
            deaths1v1 = arenaBrawlStats.value("deaths_1v1", 0);
            kills1v1 = arenaBrawlStats.value("kills_1v1", 0);
            offensive = arenaBrawlStats.value("offensive", "N/A");
            support = arenaBrawlStats.value("support", "N/A");
            utility = arenaBrawlStats.value("utility", "N/A");
            ultimate = arenaBrawlStats.value("ultimate", "N/A");
            rune = arenaBrawlStats.value("active_rune", "N/A");
            prefixColor = arenaBrawlStats.value("prefix_color", "N/A");
        }
        else {
            std::cout << "No Arena stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        int losses = losses1v1;
        int deaths = deaths1v1;
        int kills = kills1v1;

        double WLR = (losses > 0) ? wins / static_cast<double>(losses) : static_cast<double>(wins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << wins;
        std::string formatted_wins = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string colored_WLR = getArenaBrawlRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_KDR = getArenaBrawlRationColor(KDR, "KDR") + formatted_KDR;

        std::string formatte_wins = getArenaPrestigeColor(overallWins);


        if (networkLevel != -1) {
            Minecraft911->SendChatMessage("§e------------------------------------------------");
            Minecraft911->SendChatMessage("  §aArena Brawl 1v1 stats for " + formatte_wins + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft911->SendChatMessage("");
            Minecraft911->SendChatMessage("  §eOffensive: §c" + offensive + " §eUtility: §6" + utility + " §eSupport: §a" + support + " §eUltimate: §6" + ultimate + " §eRune: §4" + rune + "                  ");
            Minecraft911->SendChatMessage("  §e1v1 Wins: §6" + formatted_wins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft911->SendChatMessage("  §e1v1 Kills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft911->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft911->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft911->SendChatMessage("§aPlayer does not exist         ");
    }
}

void get2v2ArenaBrawlStats(const std::string& player) {

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

        int wins = 0, overallWins = 0;
        int losses2v2 = 0, deaths2v2 = 0, kills2v2 = 0;

        std::string offensive, support, utility, ultimate = "N/A";
        std::string rune = "N/A";
        std::string prefixColor = "N/A";

        if (playerData.contains("stats") && playerData["stats"].contains("Arena")) {
            const auto& arenaBrawlStats = playerData["stats"]["Arena"];
            overallWins = arenaBrawlStats.value("wins", 0);
            wins = arenaBrawlStats.value("wins_2v2", 0);
            losses2v2 = arenaBrawlStats.value("losses_2v2", 0);
            deaths2v2 = arenaBrawlStats.value("deaths_2v2", 0);
            kills2v2 = arenaBrawlStats.value("kills_2v2", 0);
            offensive = arenaBrawlStats.value("offensive", "N/A");
            support = arenaBrawlStats.value("support", "N/A");
            utility = arenaBrawlStats.value("utility", "N/A");
            ultimate = arenaBrawlStats.value("ultimate", "N/A");
            rune = arenaBrawlStats.value("active_rune", "N/A");
            prefixColor = arenaBrawlStats.value("prefix_color", "N/A");
        }
        else {
            std::cout << "No Arena stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        int losses = losses2v2;
        int deaths = deaths2v2;
        int kills = kills2v2;

        double WLR = (losses > 0) ? wins / static_cast<double>(losses) : static_cast<double>(wins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << wins;
        std::string formatted_wins = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string colored_WLR = getArenaBrawlRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_KDR = getArenaBrawlRationColor(KDR, "KDR") + formatted_KDR;

        std::string formatte_wins = getArenaPrestigeColor(overallWins);


        if (networkLevel != -1) {
            Minecraft911->SendChatMessage("§e------------------------------------------------");
            Minecraft911->SendChatMessage("  §aArena Brawl 2v2 stats for " + formatte_wins + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft911->SendChatMessage("");
            Minecraft911->SendChatMessage("  §eOffensive: §c" + offensive + " §eUtility: §6" + utility + " §eSupport: §a" + support + " §eUltimate: §6" + ultimate + " §eRune: §4" + rune + "                  ");
            Minecraft911->SendChatMessage("  §e2v2 Wins: §6" + formatted_wins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft911->SendChatMessage("  §e2v2 Kills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft911->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft911->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft911->SendChatMessage("§aPlayer does not exist         ");
    }
}

void get4v4ArenaBrawlStats(const std::string& player) {

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

        int wins = 0, overallWins = 0;
        int losses4v4 = 0, deaths4v4 = 0, kills4v4 = 0;

        std::string offensive, support, utility, ultimate = "N/A";
        std::string rune = "N/A";
        std::string prefixColor = "N/A";

        if (playerData.contains("stats") && playerData["stats"].contains("Arena")) {
            const auto& arenaBrawlStats = playerData["stats"]["Arena"];
            overallWins = arenaBrawlStats.value("wins", 0);
            wins = arenaBrawlStats.value("wins_4v4", 0);
            losses4v4 = arenaBrawlStats.value("losses_4v4", 0);
            deaths4v4 = arenaBrawlStats.value("deaths_4v4", 0);
            kills4v4 = arenaBrawlStats.value("kills_4v4", 0);
            offensive = arenaBrawlStats.value("offensive", "N/A");
            support = arenaBrawlStats.value("support", "N/A");
            utility = arenaBrawlStats.value("utility", "N/A");
            ultimate = arenaBrawlStats.value("ultimate", "N/A");
            rune = arenaBrawlStats.value("active_rune", "N/A");
            prefixColor = arenaBrawlStats.value("prefix_color", "N/A");
        }
        else {
            std::cout << "No Arena stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        int losses = losses4v4;
        int deaths = deaths4v4;
        int kills = kills4v4;

        double WLR = (losses > 0) ? wins / static_cast<double>(losses) : static_cast<double>(wins);
        double KDR = (deaths > 0) ? kills / static_cast<double>(deaths) : static_cast<double>(kills);

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;

        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        ss << wins;
        std::string formatted_wins = ss.str();
        ss.str("");

        ss << kills;
        std::string formatted_kills = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << WLR;
        std::string formatted_WLR = ss.str();
        ss.str("");

        ss << std::fixed << std::setprecision(2) << KDR;
        std::string formatted_KDR = ss.str();
        ss.str("");

        std::string colored_WLR = getArenaBrawlRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_KDR = getArenaBrawlRationColor(KDR, "KDR") + formatted_KDR;

        std::string formatte_wins = getArenaPrestigeColor(overallWins);


        if (networkLevel != -1) {
            Minecraft911->SendChatMessage("§e------------------------------------------------");
            Minecraft911->SendChatMessage("  §aArena Brawl 4v4 stats for " + formatte_wins + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft911->SendChatMessage("");
            Minecraft911->SendChatMessage("  §eOffensive: §c" + offensive + " §eUtility: §6" + utility + " §eSupport: §a" + support + " §eUltimate: §6" + ultimate + " §eRune: §4" + rune + "                  ");
            Minecraft911->SendChatMessage("  §e4v4 Wins: §6" + formatted_wins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft911->SendChatMessage("  §e4v4 Kills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft911->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft911->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft911->SendChatMessage("§aPlayer does not exist         ");
    }
}