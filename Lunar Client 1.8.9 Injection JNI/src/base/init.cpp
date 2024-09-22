#include "../init.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

auto Minecraft = std::make_unique<CMinecraft>();

using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string performRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

std::string getUUID(const std::string& pseudo) {
    std::string url = "https://api.mojang.com/users/profiles/minecraft/" + pseudo;
    std::string response = performRequest(url);

    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("id")) {
            return jsonResponse["id"];
        }
        else {
            return "uuidfailed";
        }
    }
    catch (const std::exception& e) {
        return "uuidfailed";
    }
}


std::string newPackageRank;
std::string monthlyPackageRank;
std::string rankPlusColor;

std::string getUserPath() {
    char userPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath))) {
        return std::string(userPath);
    }
    return "";
}

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

std::string cause;
bool start = false;
std::string apiKey;
std::string responseSuccess;

void extractSuccess(const std::string& responseSuccess) {
    try {
        auto jsonResponseSuccess = json::parse(responseSuccess);

        if (jsonResponseSuccess.contains("success") && !jsonResponseSuccess["success"].get<bool>()) {

            if (jsonResponseSuccess.contains("cause")) {
                cause = jsonResponseSuccess["cause"];

                if (cause == "Invalid API key") {
                    Minecraft->SendChatMessage("§eYou are not whitelisted                                   ");
                }
                else {
                    start = true;
                }
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        cause = "error";
    }
}


void extractPlayer(const std::string& responsePlayer) {
    try {
        auto jsonResponsePlayer = json::parse(responsePlayer);
        if (jsonResponsePlayer.contains("player")) {
            if (jsonResponsePlayer["player"].contains(Minecraft->GetLocalPlayer().GetName())) {
                apiKey = jsonResponsePlayer["player"][Minecraft->GetLocalPlayer().GetName()];
                responseSuccess = performRequest("https://api.hypixel.net/player?key=" + apiKey);
                extractSuccess(responseSuccess);

            }
            else {
                apiKey = "N/A";
                Minecraft->SendChatMessage("§eYou are not whitelisted                                   ");
            }
        }
    }
    catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        apiKey = "error";
    }

}

const std::unordered_map<std::string, std::string> newPackageRankMap = {
    {"VIP", " §a[VIP]"},
    {"VIP_PLUS", " §a[VIP§e+§a]"},
    {"MVP", " §b[MVP]"},
    {"MVP_PLUS", " §b[MVP§c+§b]"}
};

const std::unordered_map<std::string, std::string> getnewPackageRankMap() {
    return newPackageRankMap;
}

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

const std::unordered_map<std::string, std::string> getrankPlusColorMap() {
    return rankPlusColorMap;
}

const std::unordered_map<std::string, std::string> monthlyPackageRankMap = {
    {"SUPERSTAR", " §6[MVP§c++§6]"}
};

const std::unordered_map<std::string, std::string> getmonthlyPackageRankMap() {
    return monthlyPackageRankMap;
}

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

const std::unordered_map<std::string, std::string> getSuperstarPlusColorMap() {
    return superstarPlusColorMap;
}

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

std::string getApiKey() {
    return apiKey;
}
void init(void* instance) {
    jsize count;
    if (JNI_GetCreatedJavaVMs(&lc->vm, 1, &count) != JNI_OK || count == 0) {
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
        return;
    }

    if (lc->vm->GetEnv(reinterpret_cast<void**>(&lc->env), JNI_VERSION_1_8) == JNI_EDETACHED) {
        if (lc->vm->AttachCurrentThread(reinterpret_cast<void**>(&lc->env), nullptr) != JNI_OK) {
            FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
            return;
        }
    }

    if (lc->env) {
        lc->GetLoadedClasses();

        std::string userPath = getUserPath();
        std::string logFilePath = userPath + "\\.lunarclient\\offline\\multiver\\logs\\latest.log";

        std::string responsePlayer = performRequest("https://kerhosting.com/name-apikey.json");
        extractPlayer(responsePlayer);

        std::ofstream logFile(logFilePath, std::ios::out | std::ios::trunc);
        
        if (start)
        {
            std::string url = "https://api.hypixel.net/player?key=" + apiKey + "&name=" + Minecraft->GetLocalPlayer().GetName();

            std::string response = performRequest(url);
            extractRanks(response);

            std::string rankDisplay = getRankDisplay(newPackageRank, monthlyPackageRank, rankPlusColor);
            
            Minecraft->SendChatMessage("§eWelcome" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "                                   ");
            while (!GetAsyncKeyState(VK_DELETE)) {
                readLogFile(logFilePath);
                Sleep(20);
            }
            Minecraft->SendChatMessage("§eGoodbye" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "                                   ");
        }

        }
        
    else {
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
    }

    lc->vm->DetachCurrentThread();
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}