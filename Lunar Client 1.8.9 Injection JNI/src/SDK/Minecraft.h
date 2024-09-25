#pragma once
#include "Player.h"
#include <string>

class CMinecraft {
public:
    jclass GetClass();
    jobject GetInstance();
    CPlayer GetLocalPlayer();
    void SendChatMessage(const std::string& message);
    void SendPlayerChatMessage(const std::string& message);
    void SendTitleMessage(const std::string& title, const std::string& subtitle, int fadeIn, int stay, int fadeOut);
};
