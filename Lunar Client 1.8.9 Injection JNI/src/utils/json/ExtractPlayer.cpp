#include "ExtractPlayer.h"
#include "../src/utils/http/Network.h"
#include "../src/utils/json/ExtractSuccess.h"

#include <nlohmann/json.hpp>

auto Minecraft23874 = std::make_unique<CMinecraft>();

using json = nlohmann::json;

std::string apiKey;
std::string responseSuccess;

std::string getApiKey() {
    return apiKey;
}

bool extractPlayer(const std::string& responsePlayer) {
    try {
        auto jsonResponsePlayer = json::parse(responsePlayer);
        if (jsonResponsePlayer.contains("player")) {
            if (jsonResponsePlayer["player"].contains(Minecraft23874->GetLocalPlayer().GetName())) {
                apiKey = jsonResponsePlayer["player"][Minecraft23874->GetLocalPlayer().GetName()];
                responseSuccess = performRequest("https://api.hypixel.net/player?key=" + apiKey);
                return extractSuccess(responseSuccess);
            }
            else {
                apiKey = "N/A"; 
                Minecraft23874->SendChatMessage("§eYou are not whitelisted                                   ");
                return false;
            }
            getApiKey() = apiKey;
        }
        else {
            apiKey = "N/A";
            Minecraft23874->SendChatMessage("§eYou are not whitelisted                                   ");
            return false;
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        apiKey = "error";
    }
}