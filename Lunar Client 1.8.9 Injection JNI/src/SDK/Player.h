#pragma once
#include "../Java.h"

class CPlayer {
public:
    explicit CPlayer(jobject instance);
    jclass GetClass();
    void Cleanup();
    std::string GetName();
    std::string GetPlayerBrand();
    jobject GetPlayerInstance() const;
    bool isSneaking();
    bool isSprinting();
    float rotationPitch();
    float rotationYaw();
    
    void SetCursor(float p, float y);
    void setSprinting(bool s);
    void setSneaking(bool n);
private:
    jobject playerInstance;
};

inline jobject CPlayer::GetPlayerInstance() const {
    return playerInstance;
}
