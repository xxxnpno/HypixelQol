#pragma once
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
std::string performRequest(const std::string& url);
std::string getUUID(const std::string& pseudo);

