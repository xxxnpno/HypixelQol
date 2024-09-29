#include "../src/init.h"
#include "../src/stats/parsing/parse.h"
#include "sw.h"

auto Minecraft4111 = std::make_unique<CMinecraft>();


std::string getSkywarsPrestigeColor(int level) {
    static const std::map<int, std::string> prestigeColors = {
        {5, "§7"}, {10, "§f"}, {15, "§6"}, {20, "§b"}, {25, "§2"},
        {30, "§3"}, {35, "§4"}, {40, "§5"}, {45, "§1"}, {50, "§d"}
    };


    static const std::vector<std::pair<int, std::function<std::string(int)>>> specialPrestigeRanges = {
        {50, [](int lvl) { return "§c[§7" + std::to_string((lvl / 10) % 10) + "§e" + std::to_string(lvl % 10) + "§k#§r§b]"; }},
        {55, [](int lvl) { return "§8[§7" + std::to_string((lvl / 10) % 10) + "§7" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {60, [](int lvl) { return "§4[§c" + std::to_string((lvl / 10) % 10) + "§c" + std::to_string(lvl % 10) + "§k#§r§4]"; }},
        {65, [](int lvl) { return "§c[§f" + std::to_string((lvl / 10) % 10) + "§f" + std::to_string(lvl % 10) + "§k#§r§c]"; }},
        {70, [](int lvl) { return "§e[§6" + std::to_string((lvl / 10) % 10) + "§6" + std::to_string(lvl % 10) + "§k#§r§e]"; }},
        {75, [](int lvl) { return "§f[§9" + std::to_string((lvl / 10) % 10) + "§9" + std::to_string(lvl % 10) + "§k#§r§f]"; }},
        {80, [](int lvl) { return "§f[§b" + std::to_string((lvl / 10) % 10) + "§b" + std::to_string(lvl % 10) + "§k#§r§f]"; }},
        {85, [](int lvl) { return "§f[§3" + std::to_string((lvl / 10) % 10) + "§3" + std::to_string(lvl % 10) + "§k#§r§f]"; }},
        {90, [](int lvl) { return "§a[§3" + std::to_string((lvl / 10) % 10) + "§3" + std::to_string(lvl % 10) + "§k#§r§a]"; }},
        {95, [](int lvl) { return "§c[§e" + std::to_string((lvl / 10) % 10) + "§e" + std::to_string(lvl % 10) + "§k#§r§c]"; }},
        {100, [](int lvl) { return "§1[§1" + std::to_string((lvl / 100) % 10) + "§1" + std::to_string((lvl / 10) % 10) + "§1" + std::to_string(lvl % 10) + "§k#§r§1]"; }},
        {105, [](int lvl) { return "§6[§4" + std::to_string((lvl / 100) % 10) + "§4" + std::to_string((lvl / 10) % 10) + "§4" + std::to_string(lvl % 10) + "§k#§r§6]"; }},
        {110, [](int lvl) { return "§1[§b" + std::to_string((lvl / 100) % 10) + "§1" + std::to_string((lvl / 10) % 10) + "§1" + std::to_string(lvl % 10) + "§k#§r§b]"; }},
        {115, [](int lvl) { return "§8[§7" + std::to_string((lvl / 100) % 10) + "§7" + std::to_string((lvl / 10) % 10) + "§7" + std::to_string(lvl % 10) + "§k#§r§8]"; }},
        {120, [](int lvl) { return "§d[§d" + std::to_string((lvl / 100) % 10) + "§d" + std::to_string((lvl / 10) % 10) + "§d" + std::to_string(lvl % 10) + "§k#§r§d]"; }},
        {125, [](int lvl) { return "§7[§e" + std::to_string((lvl / 100) % 10) + "§e" + std::to_string((lvl / 10) % 10) + "§e" + std::to_string(lvl % 10) + "§k#§r§7]"; }},
        {130, [](int lvl) { return "§4[§e" + std::to_string((lvl / 100) % 10) + "§e" + std::to_string((lvl / 10) % 10) + "§e" + std::to_string(lvl % 10) + "§k#§r§4]"; }},
        {135, [](int lvl) { return "§7[§c" + std::to_string((lvl / 100) % 10) + "§c" + std::to_string((lvl / 10) % 10) + "§c" + std::to_string(lvl % 10) + "§k#§r§7]"; }},
        {140, [](int lvl) { return "§a[§c" + std::to_string((lvl / 100) % 10) + "§c" + std::to_string((lvl / 10) % 10) + "§c" + std::to_string(lvl % 10) + "§k#§r§a]"; }},
        {145, [](int lvl) { return "§a[§b" + std::to_string((lvl / 100) % 10) + "§b" + std::to_string((lvl / 10) % 10) + "§b" + std::to_string(lvl % 10) + "§k#§r§b]"; }},
                                      
    };

    for (const auto& [minLevel, color] : prestigeColors) {
        if (level < minLevel) {
            return color + " [" + std::to_string(level) + "§k#§r" + color + "]";
        }
    }

    for (const auto& [minLevel, formatFunc] : specialPrestigeRanges) {
        if (level >= minLevel && level < minLevel + 5) { 
            return formatFunc(level);
        }
    }

    return "§l§c§k#§r§6" + std::to_string((level / 100) % 10) + "§e" + std::to_string((level / 10) % 10) + "§a" + std::to_string(level % 10) + "§c§k#§r";
}

std::string getSkywarsRationColor(double ratio, const std::string& type) {
    if (type == "WLR") {
        if (ratio >= 5.00) return "§0";
        if (ratio >= 2.00) return "§4";
        if (ratio >= 1.10) return "§c";
        if (ratio >= 0.80) return "§e";
        if (ratio >= 0.40) return "§2";
        if (ratio >= 0.20) return "§a";
        if (ratio >= 0.00) return "§f";
        return "§7";
    }
    else if (type == "KDR") {
        if (ratio >= 10.00) return "§0";
        if (ratio >= 4.00) return "§4";
        if (ratio >= 2.50) return "§c";
        if (ratio >= 1.70) return "§2";
        if (ratio >= 1.00) return "§a";
        if (ratio >= 0.50) return "§f";
        return "§7";
    }
    return "§7";
}

void getSwStats(const std::string& player) {

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

        int losses = 0;
        int deaths = 0;
        int kills = 0;

        if (playerData.contains("stats") && playerData["stats"].contains("SkyWars")) {
            const auto& skywarsStats = playerData["stats"]["SkyWars"];
            losses = skywarsStats.value("losses", 0);
            deaths = skywarsStats.value("deaths", 0);
            kills = skywarsStats.value("kills", 0);
        }
        else {
            std::cout << "No SkyWars stats found for player." << std::endl;
        }

        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");
        int soloWins = playerData.value("achievements", json::object()).value("skywars_wins_solo", 0);
        int teamWins = playerData.value("achievements", json::object()).value("skywars_wins_team", 0);
        int level = playerData.value("achievements", json::object()).value("skywars_you_re_a_star", 0);

        int wins = soloWins + teamWins;

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

        std::string colored_WLR = getSkywarsRationColor(WLR, "WLR") + formatted_WLR;
        std::string colored_KDR = getSkywarsRationColor(KDR, "KDR") + formatted_KDR;

        std::string formatte_level = getSkywarsPrestigeColor(level);

        if (networkLevel != -1) {
            Minecraft4111->SendChatMessage("§e------------------------------------------------");
            Minecraft4111->SendChatMessage("  §aSkywars stats for "+ formatte_level + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft4111->SendChatMessage("");
            Minecraft4111->SendChatMessage("  §eWins: §6" + formatted_wins + "§e - WLR: " + colored_WLR + "                  ");
            Minecraft4111->SendChatMessage("  §eKills: §6" + formatted_kills + "§e - KDR: " + colored_KDR + "                  ");
            Minecraft4111->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft4111->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft4111->SendChatMessage("§aPlayer does not exist.");
    }
}