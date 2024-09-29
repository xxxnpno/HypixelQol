#include "../src/init.h"
#include "../src/stats/parsing/parse.h"
#include "stalk.h"

auto Minecraft4234 = std::make_unique<CMinecraft>();

void stalk(const std::string& player) {

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


        std::string newPackageRank = playerData.value("newPackageRank", "N/A");
        std::string monthlyPackageRank = playerData.value("monthlyPackageRank", "N/A");
        std::string rankPlusColor = playerData.value("rankPlusColor", "N/A");

        double networkExp = playerData.value("networkExp", -1.0);
        int networkLevel = (networkExp != -1) ? static_cast<int>(floor((sqrt((2 * networkExp) + 30625) / 50) - 2.5)) : -1;


        std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);

        const std::string statusUrl = "https://api.hypixel.net/status?key=" + getApiKey() + "&uuid=" + uuid;
        std::string statusResponse = performRequest(statusUrl);

        auto statusJson = json::parse(statusResponse);

        std::string gameType = "N/A", mode = "N/A", map = "N/A";
        if (statusJson.contains("session") && statusJson["session"].contains("online") && statusJson["session"]["online"].get<bool>()) {
            gameType = statusJson["session"].value("gameType", "N/A");
            mode = statusJson["session"].value("mode", "N/A");
            map = statusJson["session"].value("map", "N/A");
        }

        std::stringstream ss;
        ss.imbue(std::locale("en_US.UTF-8"));

        ss << networkLevel;
        std::string formatted_networkLevel = ss.str();
        ss.str("");

        if (networkLevel != -1) {
            Minecraft4234->SendChatMessage("§e------------------------------------------------");
            Minecraft4234->SendChatMessage("  §aStatus for" + rankDisplay + " " + player + " §a-  §3" + formatted_networkLevel + "                  ");
            Minecraft4234->SendChatMessage("");
            Minecraft4234->SendChatMessage("  §eGame Type: §a" + gameType + "                  ");
            Minecraft4234->SendChatMessage("  §eMode: §a" + mode + "                  ");
            Minecraft4234->SendChatMessage("  §eMap: §a" + map + "                  ");
            Minecraft4234->SendChatMessage("§e------------------------------------------------");
        }
        else {
            Minecraft4234->SendChatMessage("§aPlayer never joined Hypixel          ");
        }
    }
    else {
        Minecraft4234->SendChatMessage("§aPlayer does not exist.");
    }
}