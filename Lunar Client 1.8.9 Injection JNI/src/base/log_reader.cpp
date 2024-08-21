#include "../log_reader.h"

std::unordered_set<std::string> seenLines;
auto Minecraft2 = std::make_unique<CMinecraft>();

std::string generateRandomCaseMessage(const std::string& message) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::string result = message;
    for (char& c : result) {
        if (std::isalpha(c)) {
            c = dis(gen) ? std::toupper(c) : std::tolower(c);
        }
    }
    return result;
}

int countColons(const std::string& line) {
    return std::count(line.begin(), line.end(), ':');
}

void readLogFile(const std::string& filePath) {
    std::ifstream logFile(filePath);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        return;
    }

    std::string line;
    bool goodGameSent = false;


    std::regex winnerPatterns[] = {
        std::regex(R"(\[CHAT\].*WINNER!)"),
        std::regex(R"(\[CHAT\].*Winner)"),
        std::regex(R"(\[CHAT\].*VICTORY)"),
        std::regex(R"(\[CHAT\].*Winning)"),
    };

    std::regex winnerPatterns2[] = {
        std::regex(R"(\[CHAT\].*1st Killer:)"),
        std::regex(R"(\[CHAT\].*1st Place:)"),
        std::regex(R"(\[CHAT\].*Winner:)")
    };

    while (std::getline(logFile, line)) {
        if (seenLines.insert(line).second) {
            std::cout << line << std::endl;

            if (!goodGameSent && countColons(line) < 4) {
                for (const auto& pattern : winnerPatterns) {
                    if (std::regex_search(line, pattern)) {
                        std::string message = generateRandomCaseMessage("good game");
                        Minecraft2->SendPlayerChatMessage("/ac " + message);
                        goodGameSent = true;
                        break;
                    }
                }
            }

            if (!goodGameSent) {
                for (const auto& pattern : winnerPatterns2) {
                    if (std::regex_search(line, pattern)) {
                        std::string message = generateRandomCaseMessage("good game");
                        Minecraft2->SendPlayerChatMessage("/ac " + message);
                        goodGameSent = true;
                        break;
                    }
                }
            }
        }
    }
}
