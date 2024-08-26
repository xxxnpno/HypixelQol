#include "../log_reader.h"
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <vector>

std::unordered_set<std::string> seenLines;
std::deque<std::string> recentMessages;
const size_t maxRecentMessages = 5;
auto Minecraft2 = std::make_unique<CMinecraft>();

std::vector<std::string> accuracyMessages = {
    "Wow, your Accuracy is so good, it must be the end of the game!",
    "Did you see that Accuracy? Game over, man, just leave now!",
    "My Accuracy is off the charts, guess it's time to disconnect!",
    "With this kind of Accuracy, why even continue? GG!",
    "Accuracy 100%, no need to keep playing, right?",
    "Your Accuracy was so bad, I think the game ended!",
    "This Accuracy level just broke the game, might as well log out.",
    "The Accuracy is unbeatable, time to call it quits!",
    "Accuracy achieved, I think Hypixel just declared me the winner!",
    "With such Accuracy, it's like the duel ended before it began!"
};

int currentMessageIndex = 0;

std::string generateRandomCaseMessage(const std::string& message) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::string result;
    do {
        result = message;
        for (char& c : result) {
            if (std::isalpha(c)) {
                c = dis(gen) ? std::toupper(c) : std::tolower(c);
            }
        }
    } while (std::find(recentMessages.begin(), recentMessages.end(), result) != recentMessages.end());

    return result;
}

void storeRecentMessage(const std::string& message) {
    if (recentMessages.size() >= maxRecentMessages) {
        recentMessages.pop_front();
    }
    recentMessages.push_back(message);
}

int countColons(const std::string& line) {
    return std::count(line.begin(), line.end(), ':');
}

int counttiretdu6(const std::string& line) {
    return std::count(line.begin(), line.end(), '-');
}

void sendAccuracyMessage() {
    std::string message = accuracyMessages[currentMessageIndex];
    Minecraft2->SendPlayerChatMessage("/ac " + message);

    storeRecentMessage(message);

    currentMessageIndex = (currentMessageIndex + 1) % accuracyMessages.size();
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
        std::regex(R"(\[CHAT\].*Draw)"),
        std::regex(R"(\[CHAT\].*Winners)")
    };

    std::regex winnerPatterns2[] = {
        std::regex(R"(\[CHAT\].*1st Killer:)"),
        std::regex(R"(\[CHAT\].*1st Place:)"),
        std::regex(R"(\[CHAT\].*Winner:)")
    };

    std::regex accuracyPatern[] = {
        std::regex(R"(\[CHAT\].*OP Duel)"),
        std::regex(R"(\[CHAT\].*Sumo Duel)"),
        std::regex(R"(\[CHAT\].*Boxing Duel)"),
        std::regex(R"(\[CHAT\].*Classic Duel)")
    };

    while (std::getline(logFile, line)) {
        if (seenLines.insert(line).second) {
            std::cout << line << std::endl;

            if (counttiretdu6(line) == 0 && countColons(line) < 4) {
                for (const auto& pattern : accuracyPatern) {
                    if (std::regex_search(line, pattern)) {
                        Sleep(100);
                        sendAccuracyMessage();  
                        break;
                    }
                }
            }

            if (!goodGameSent && countColons(line) < 4) {
                for (const auto& pattern : winnerPatterns) {
                    if (std::regex_search(line, pattern)) {
                        std::string message = generateRandomCaseMessage("good game");
                        Minecraft2->SendPlayerChatMessage("/ac " + message);
                        storeRecentMessage(message);
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
                        storeRecentMessage(message);
                        goodGameSent = true;
                        break;
                    }
                }
            }
        }
    }
}
