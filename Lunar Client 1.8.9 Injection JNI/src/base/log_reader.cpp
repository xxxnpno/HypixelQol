#include "../log_reader.h"

std::unordered_set<std::string> seenLines;

void readLogFile(const std::string& filePath) {
    std::ifstream logFile(filePath);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(logFile, line)) {
        if (seenLines.find(line) == seenLines.end()) {
            std::cout << line << std::endl;
            seenLines.insert(line);
        }
    }
}