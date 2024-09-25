#include "Network.h"
#include <iostream>

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
