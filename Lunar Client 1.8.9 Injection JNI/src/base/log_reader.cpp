#include "../log_reader.h"

#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <unordered_set>
#include <deque>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

using json = nlohmann::json;

std::unordered_set<std::string> seenLines;
std::deque<std::string> recentMessages;
const size_t maxRecentMessages = 5;
auto Minecraft2 = std::make_unique<CMinecraft>();

std::string lastGametype;
std::string lastMode;

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

void handleGameInfoMessage(const std::string& jsonMessage) {
    try {
        auto parsed = json::parse(jsonMessage);

        lastGametype = parsed.value("gametype", "");
        lastMode = parsed.value("mode", "");

    }
    catch (const json::parse_error& e) {
        std::cerr << "Failed to parse JSON message: " << e.what() << std::endl;
    }
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
        std::regex(R"(\[CHAT\].*1st Killer)"),
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

    std::regex requeuePattern(R"(Unknown command\. Type \"\/help\" for help\. \('rq'\))");
    std::regex playerRqPattern(R"(\[CHAT\] .*?(\S+): \!rq)");
    std::smatch matches;
    bool correct = false;

    std::regex swPattern(R"(Unknown command\. Type "help" for help\. \('sw (\w+)'\))");
    std::regex swPattern2(R"(Unknown command\. Type \"\/help\" for help\. \('sw'\))");

    std::regex bwPattern(R"(Unknown command\. Type "help" for help\. \('bw (\w+)'\))");
    std::regex bwPattern2(R"(Unknown command\. Type \"\/help\" for help\. \('bw'\))");

    std::regex generalPattern(R"(Unknown command\. Type "/help" for help\. \('general (\w+)'\))");
    std::regex generalPattern2(R"(Unknown command\. Type \"\/help\" for help\. \('general'\))");


    while (std::getline(logFile, line)) {
        if (seenLines.insert(line).second) {
            //std::cout << line << '\n';
            if (std::regex_search(line, matches, playerRqPattern) && matches.size() > 1) {
                std::string pseudo = matches[1].str();
                if (pseudo == Minecraft2->GetLocalPlayer().GetName()) {
                    correct = true;
                }
                else {
                    correct = false;
                }
            }

            if (std::regex_search(line, requeuePattern) || correct) {
                if (!lastGametype.empty() && !lastMode.empty()) {
                    std::map<std::pair<std::string, std::string>, std::string> commandMap = {
                        {{"ARENA", "1v1"}, "/play arena_1v1"},
                        {{"ARENA", "2v2"}, "/play arena_2v2"},
                        {{"ARENA", "4v4"}, "/play arena_4v4"},
                        {{"VAMPIREZ", "normal"}, "/play vampirez"},
                        {{"GINGERBREAD", "normal"}, "/play tkr"},
                        {{"PAINTBALL", "normal"}, "/play paintball"},
                        {{"WALLS", "normal"}, "/play walls"},
                        {{"QUAKECRAFT", "teams"}, "/play quake_teams"},
                        {{"QUAKECRAFT", "solo"}, "/play quake_solo"},
                        {{"MCGO", "deathmatch"}, "/play mcgo_deathmatch"},
                        {{"MCGO", "gungame"}, "/play mcgo_gungame"},
                        {{"MCGO", "normal"}, "/play mcgo_normal"},
                        {{"WALLS3", "face_off"}, "/play mw_face_off"},
                        {{"WALLS3", "standard"}, "/play mw_standard"},
                        {{"SUPER_SMASH", "solo_normal"}, "/play super_smash_solo_normal"},
                        {{"SUPER_SMASH", "team_normal"}, "/play super_smash_teams_normal"},
                        {{"SUPER_SMASH", "1v1_normal"}, "/play super_smash_1v1_normal"},
                        {{"SUPER_SMASH", "2v2_normal"}, "/play super_smash_2v2_normal"},
                        {{"BATTLEGROUND", "ctf_mini"}, "/play warlords_ctf_mini"},
                        {{"BATTLEGROUND", "domination"}, "/play warlords_domination"},
                        {{"BATTLEGROUND", "team_deathmatch"}, "/play warlords_team_deathmatch"},
                        {{"SURVIVAL_GAMES", "teams_normal"}, "/play blitz_teams_normal"},
                        {{"SURVIVAL_GAMES", "solo_normal"}, "/play blitz_solo_normal"},
                        {{"SPEED_UHC", "solo_normal"}, "/play speed_solo_normal"},
                        {{"SPEED_UHC", "team_normal"}, "/play speed_team_normal"},
                        {{"UHC", "SOLO"}, "/play uhc_solo"},
                        {{"UHC", "TEAMS"}, "/play uhc_teams"},
                        {{"TNTGAMES", "BOWSPLEEF"}, "/play tnt_bowspleef"},
                        {{"TNTGAMES", "PVPRUN"}, "/play tnt_pvprun"},
                        {{"TNTGAMES", "TNTRUN"}, "/play tnt_tntrun"},
                        {{"TNTGAMES", "TNTAG"}, "/play tnt_tntag"},
                        {{"TNTGAMES", "CAPTURE"}, "/play tnt_capture"},
                        {{"BUILD_BATTLE", "BUILD_BATTLE_SOLO_PRO"}, "/play build_battle_solo_pro"},
                        {{"BUILD_BATTLE", "BUILD_BATTLE_SOLO_NORMAL"}, "/play build_battle_solo_normal"},
                        {{"BUILD_BATTLE", "BUILD_BATTLE_TEAMS_NORMAL"}, "/play build_battle_teams_normal"},
                        {{"BUILD_BATTLE", "BUILD_BATTLE_GUESS_THE_BUILD"}, "/play build_battle_guess_the_build"},
                        {{"MURDER_MYSTERY", "MURDER_ASSASSINS"}, "/play murder_classic"},
                        {{"MURDER_MYSTERY", "MURDER_INFECTION"}, "/play murder_infection"},
                        {{"MURDER_MYSTERY", "MURDER_CLASSIC"}, "/play murder_classic"},
                        {{"MURDER_MYSTERY", "MURDER_DOUBLE_UP"}, "/play murder_double_up"},
                        {{"SKYWARS", "solo_normal"}, "/play solo_normal"},
                        {{"SKYWARS", "solo_insane"}, "/play solo_insane"},
                        {{"SKYWARS", "teams_normal"}, "/play teams_normal"},
                        {{"SKYWARS", "teams_insane"}, "/play teams_insane"},
                        {{"SKYWARS", "solo_insane_lucky"}, "/play solo_insane_lucky"},
                        {{"SKYWARS", "teams_insane_normal"}, "/play teams_insane_normal"},
                        {{"SKYWARS", "mega_normal"}, "/play mega_normal"},
                        {{"SKYWARS", "mega_doubles"}, "/play mega_doubles"},
                        {{"SKYWARS", "solo_insane_tnt_madness"}, "/play solo_insane_tnt_madness"},
                        {{"SKYWARS", "teams_insane_tnt_madness"}, "/play teams_insane_tnt_madness"},
                        {{"SKYWARS", "solo_insane_rush"}, "/play solo_insane_rush"},
                        {{"SKYWARS", "teams_insane_rush"}, "/play teams_insane_rush"},
                        {{"SKYWARS", "solo_insane_slime"}, "/play solo_insane_slime"},
                        {{"SKYWARS", "teams_insane_slime"}, "/play teams_insane_slime"},
                        {{"SKYWARS", "solo_insane_hunters_vs_beasts"}, "/play solo_insane_hunters_vs_beasts"},
                        {{"WOOL_GAMES", "capture_the_wool_two_twenty"}, "/play wool_capture_the_wool_two_twenty"},
                        {{"WOOL_GAMES", "sheep_wars_two_six"}, "/play wool_sheep_wars_two_six"},
                        {{"WOOL_GAMES", "wool_wars_two_four"}, "/play wool_wool_wars_two_four"},
                        {{"BEDWARS", "BEDWARS_EIGHT_ONE"}, "/play bedwars_eight_one"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO"}, "/play bedwars_eight_two"},
                        {{"BEDWARS", "BEDWARS_FOUR_THREE"}, "/play bedwars_four_three"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR"}, "/play bedwars_four_four"},
                        {{"BEDWARS", "BEDWARS_CAPTURE"}, "/play bedwars_capture"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO_RUSH"}, "/play bedwars_eight_two_rush"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR_RUSH"}, "/play bedwars_four_four_rush"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO_ULTIMATE"}, "/play bedwars_eight_two_ultimate"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR_ULTIMATE"}, "/play bedwars_four_four_ultimate"},
                        {{"BEDWARS", "BEDWARS_CASTLE"}, "/play bedwars_castle"},
                        {{"BEDWARS", "BEDWARS_TWO_FOUR"}, "/play bedwars_two_four"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO_VOIDLESS"}, "/play bedwars_eight_two_voidless"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR_VOIDLESS"}, "/play bedwars_four_four_voidless"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO_ARMED"}, "/play bedwars_eight two_armed"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR_ARMED"}, "/play bedwars_four_four_armed"},
                        {{"BEDWARS", "BEDWARS_EIGHT_TWO_LUCKY"}, "/play bedwars_eight two_lucky"},
                        {{"BEDWARS", "BEDWARS_FOUR_FOUR_LUCKY"}, "/play bedwars_four_four_lucky"},
                        {{"ARCADE", "HOLE_IN_THE_WALL"}, "/play arcade_hole_in_the_wall"},
                        {{"ARCADE", "SOCCER"}, "/play arcade_soccer"},
                        {{"ARCADE", "BOUNTY_HUNTER"}, "/play arcade_bounty_hunters"},
                        {{"ARCADE", "PIXEL_PAINTERS"}, "/play arcade_pixel_painters"},
                        {{"ARCADE", "DRAGON_WARS"}, "/play arcade_dragon_wars"},
                        {{"ARCADE", "ENDER_SPLEEF"}, "/play arcade_ender_spleef"},
                        {{"ARCADE", "STARWARS"}, "/play arcade_starwars"},
                        {{"ARCADE", "THROW_OUT"}, "/play arcade_throw_out"},
                        {{"ARCADE", "CREEPER_ATTACK"}, "/play arcade_creeper_attack"},
                        {{"ARCADE", "PARTY_GAMES_1"}, "/play arcade_party_games_1"},
                        {{"ARCADE", "FARM_HUNT"}, "/play arcade_farm_hunt"},
                        {{"ARCADE", "ZOMBIES_DEAD_END"}, "/play arcade_zombies_dead_end"},
                        {{"ARCADE", "ZOMBIES_BAD_BLOOD"}, "/play arcade_zombies_bad_blood"},
                        {{"ARCADE", "ZOMBIES_ALIEN_ARCADIUM"}, "/play arcade_zombies_alien_arcadium"},
                        {{"ARCADE", "ZOMBIES_PRISON"}, "/play arcade_zombies_prison"},
                        {{"ARCADE", "HIDE_AND_SEEK_PROP_HUNT"}, "/play arcade_hide_and_seek_prop_hunt"},
                        {{"ARCADE", "HIDE_AND_SEEK_PARTY_POOPER"}, "/play arcade_hide_and_seek_party_pooper"},
                        {{"ARCADE", "SIMON_SAYS"}, "/play arcade_simon_says"},
                        {{"ARCADE", "SANTA_SAYS"}, "/play arcade_santa_says"},
                        {{"ARCADE", "MINI_WALLS"}, "/play arcade_mini_walls"},
                        {{"ARCADE", "DAY_ONE"}, "/play arcade_day_one"},
                        {{"ARCADE", "PIXEL_PARTY"}, "/play arcade_pixel_party"},
                        {{"DUELS", "DUELS_CLASSIC_DUEL"}, "/play duels_classic_duel"},
                        {{"DUELS", "DUELS_SW_DUEL"}, "/play duels_sw_duel"},
                        {{"DUELS", "DUELS_SW_DOUBLES"}, "/play duels_sw_doubles"},
                        {{"DUELS", "DUELS_BOW_DUEL"}, "/play duels_bow_duel"},
                        {{"DUELS", "DUELS_UHC_DUEL"}, "/play duels_uhc_duel"},
                        {{"DUELS", "DUELS_UHC_DOUBLES"}, "/play duels_uhc_doubles"},
                        {{"DUELS", "DUELS_UHC_FOUR"}, "/play duels_uhc_four"},
                        {{"DUELS", "DUELS_UHC_MEETUP"}, "/play duels_uhc_meetup"},
                        {{"DUELS", "DUELS_POTION_DUEL"}, "/play duels_potion_duel"},
                        {{"DUELS", "DUELS_COMBO_DUEL"}, "/play duels_combo_duel"},
                        {{"DUELS", "DUELS_POTION_DUEL"}, "/play duels_potion_duel"},
                        {{"DUELS", "DUELS_OP_DUEL"}, "/play duels_op_duel"},
                        {{"DUELS", "DUELS_OP_DOUBLES"}, "/play duels_op_doubles"},
                        {{"DUELS", "DUELS_MW_DUEL"}, "/play duels_mw_duel"},
                        {{"DUELS", "DUELS_MW_DOUBLES"}, "/play duels_mw_doubles"},
                        {{"DUELS", "DUELS_SUMO_DUEL"}, "/play duels_sumo_duel"},
                        {{"DUELS", "DUELS_BLITZ_DUEL"}, "/play duels_blitz_duel"},
                        {{"DUELS", "DUELS_BOWSPLEEF_DUEL"}, "/play duels_bowspleef_duel"},
                        {{"DUELS", "DUELS_BRIDGE_DUEL"}, "/play duels_bridge_duel"},
                        {{"DUELS", "DUELS_BRIDGE_DOUBLES"}, "/play duels_bridge_doubles"},
                        {{"DUELS", "DUELS_BRIDGE_THREES"}, "/play duels_bridge_threes"},
                        {{"DUELS", "DUELS_BRIDGE_FOUR"}, "/play duels_bridge_four"},
                        {{"DUELS", "DUELS_BRIDGE_2V2V2V2"}, "/play duels_bridge_2v2v2v2"},
                        {{"DUELS", "DUELS_BRIDGE_3V3V3V3"}, "/play duels_bridge_3v3v3v3"},
                        {{"DUELS", "DUELS_CAPTURE_THREES"}, "/play duels_capture_threes"},
                        {{"DUELS", "DUELS_BOXING_DUEL"}, "/play duels_boxing_duel"},
                        {{"DUELS", "DUELS_PARKOUR_EIGHT"}, "/play duels_parkour_eight"},
                        {{"DUELS", "DUELS_DUEL_ARENA"}, "/play duels_duel_arena"} // <3
                        // /play mcgo_deathmatch_party
                        // /play mcgo_normal_party
                        // /play super_smash_friends_normal
                    };

                    auto it = commandMap.find({ lastGametype, lastMode });
                    if (it != commandMap.end()) {
                        Minecraft2->SendPlayerChatMessage(it->second);
                    }
                }
            }

            if (counttiretdu6(line) == 0 && countColons(line) < 4) {
                for (const auto& pattern : accuracyPatern) {
                    if (std::regex_search(line, pattern)) {
                        Sleep(100);
                        sendAccuracyMessage();
                        break;
                    }
                }
            }

            std::regex jsonMessagePattern(R"(\[CHAT\] \{.*\})");
            std::smatch match;
            if (std::regex_search(line, match, jsonMessagePattern)) {
                std::string jsonMessage = match.str().substr(7);
                handleGameInfoMessage(jsonMessage);
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

            std::string player;

            if (std::regex_search(line, matches, swPattern)) {
                if (matches.size() > 1) {
                    player = matches[1].str();
                    getSwStats(player);
                }
            }

            if (std::regex_search(line, matches, swPattern)) {
                player = Minecraft2->GetLocalPlayer().GetName();
                getSwStats(player);
            }

            if (std::regex_search(line, matches, bwPattern)) {
                if (matches.size() > 1) {
                    player = matches[1].str();
                    getBwStats(player);
                }
            }

            if (std::regex_search(line, matches, bwPattern)) {
                player = Minecraft2->GetLocalPlayer().GetName();
                getBwStats(player);
            }

            if (std::regex_search(line, matches, generalPattern)) {
                if (matches.size() > 1) {
                    player = matches[1].str();
                    std::cout << "Getting general stats for " << player << "...\n";
                    getGeneralStats(player);
                }
            }

            if (std::regex_search(line, matches, generalPattern2)) {
                player = Minecraft2->GetLocalPlayer().GetName();
                getGeneralStats(player);
            }
        }
    }
}
