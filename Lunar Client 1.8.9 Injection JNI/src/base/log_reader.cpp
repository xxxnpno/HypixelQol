#include "../log_reader.h"

std::unordered_set<std::string> seenLines;

auto Minecraft2 = std::make_unique<CMinecraft>();

void readLogFile(const std::string& filePath) {
    std::ifstream logFile(filePath);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

    std::string line;
    std::regex joinPattern(R"(\[CHAT\] Friend > (\w+) joined\.)");

    while (std::getline(logFile, line)) {
        if (seenLines.find(line) == seenLines.end()) {
            std::cout << line << std::endl;
            seenLines.insert(line);
            std::smatch match;
            if (std::regex_search(line, match, joinPattern)) {
                if (match.size() > 1) {
                    std::string player = match.str(1);
                    Minecraft2->SendPlayerChatMessage("/boop " + player);
                }
            }
        }
    }
}



