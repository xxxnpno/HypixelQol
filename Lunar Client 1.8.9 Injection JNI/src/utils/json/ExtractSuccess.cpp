#include "ExtractSuccess.h"
#include "../src/SDK/Minecraft.h"

#include <nlohmann/json.hpp>

auto Minecraft34385763685 = std::make_unique<CMinecraft>();

using json = nlohmann::json;

std::string cause;

bool extractSuccess(const std::string& responseSuccess) {
    try {
        auto jsonResponseSuccess = json::parse(responseSuccess);

        if (jsonResponseSuccess.contains("success") && !jsonResponseSuccess["success"].get<bool>()) {

            if (jsonResponseSuccess.contains("cause")) {
                cause = jsonResponseSuccess["cause"];

                if (cause == "Invalid API key") {
                    Minecraft34385763685->SendChatMessage("§eYou are not whitelisted                                   ");
                    return false;
                }
                else {
                    return true;
                }
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        cause = "error";
    }
}