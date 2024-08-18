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
};
