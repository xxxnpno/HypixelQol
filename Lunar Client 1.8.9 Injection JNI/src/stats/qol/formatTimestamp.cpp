#include "formatTimestamp.h"
#include <ctime>

std::string formatTimestamp(long long timestamp) {
    if (timestamp < 0) return "N/A";
    std::time_t timeInSeconds = timestamp / 1000;
    std::tm tm{};
    localtime_s(&tm, &timeInSeconds);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}