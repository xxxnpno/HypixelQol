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

    std::regex friendJoinPattern(R"(\[CHAT\] (Friend|F) > (\w+) joined\.)");

    std::regex winnerPatternDuels(R"(\[CHAT\].*WINNER!)");
    std::regex winnerPatternMurder(R"(\[CHAT\].*Winner)");
    std::regex winnerPatternSkywarsBedwars(R"(\[CHAT\].*Killer)");
    std::regex winnerPatternAB(R"(\[CHAT\].*VICTORY)");
    std::regex winnerPatternTNTRun(R"(\[CHAT\].*Place)");
    std::regex winnerPatternBSG(R"(\[CHAT\].*Winning)");

    while (std::getline(logFile, line)) {
        if (seenLines.find(line) == seenLines.end()) {
            std::cout << line << std::endl;
            seenLines.insert(line);

            std::smatch match;
            if (std::regex_search(line, match, friendJoinPattern)) {
                if (match.size() > 2) {
                    std::string player = match.str(2);
                    Minecraft2->SendPlayerChatMessage("/boop " + player);
                }
            }

            if (!goodGameSent &&
                (std::regex_search(line, winnerPatternDuels) || std::regex_search(line, winnerPatternTNTRun) ||
                    std::regex_search(line, winnerPatternAB) || std::regex_search(line, winnerPatternMurder) ||
                    std::regex_search(line, winnerPatternBSG) || std::regex_search(line, winnerPatternSkywarsBedwars)) &&
                countColons(line) < 4)
            {
                std::string message = generateRandomCaseMessage("good game");
                Minecraft2->SendPlayerChatMessage("/ac " + message);
                goodGameSent = true;  
            }
        }
    }
}
