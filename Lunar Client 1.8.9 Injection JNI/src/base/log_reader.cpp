#include "../log_reader.h"

std::unordered_set<std::string> seenLines;


void handleLogLine(const std::string& line) {
    std::regex jsonPattern(R"(\[CHAT\] (\{.*\}))");
    std::smatch match;

    if (std::regex_search(line, match, jsonPattern)) {
        std::string jsonStr = match[1].str();

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;

        std::istringstream s(jsonStr);
        if (Json::parseFromStream(reader, s, &root, &errors)) {
            std::string gametype = root.get("gametype", "").asString();
            std::string mode = root.get("mode", "").asString();

            std::string chatMessage = gametype + mode;
            Minecraft->SendChatMessage(chatMessage);
        }
        else {
            std::cerr << "Erreur de parsing JSON: " << errors << std::endl;
        }
    }
}

void readLogFile(const std::string& filePath) {
    std::ifstream logFile(filePath);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(logFile, line)) {
        if (seenLines.find(line) == seenLines.end()) {
            seenLines.insert(line);

            handleLogLine(line);

            std::cout << line << std::endl;
        }
    }
}
