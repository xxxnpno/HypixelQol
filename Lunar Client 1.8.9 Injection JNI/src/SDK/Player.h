#pragma once
#include "../Java.h"

class CPlayer {
public:
    explicit CPlayer(jobject instance);
    jclass GetClass();
    void Cleanup();
    std::string GetName();
    jobject GetPlayerInstance() const;

private:
    jobject playerInstance;
};

inline jobject CPlayer::GetPlayerInstance() const {
    return playerInstance;
}
