#include "Player.h"

CPlayer::CPlayer(jobject instance) : playerInstance(instance) {
    std::cout << "CPlayer instance created." << std::endl;
}

void CPlayer::Cleanup() {
    if (playerInstance) {
        lc->env->DeleteLocalRef(playerInstance);
        std::cout << "CPlayer instance cleaned up." << std::endl;
    }
    else {
        std::cerr << "CPlayer instance is null, nothing to clean up." << std::endl;
    }
}

jclass CPlayer::GetClass() {
    jclass cls = lc->GetClass("net.minecraft.entity.Entity");
    if (cls) {
        std::cout << "CPlayer class obtained successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to obtain CPlayer class." << std::endl;
    }
    return cls;
}

std::string CPlayer::GetName() {
    jclass entityPlayerSPClass = lc->GetClass("net.minecraft.client.entity.EntityPlayerSP");
    if (!entityPlayerSPClass) {
        std::cerr << "EntityPlayerSP class not found." << std::endl;
        return "";
    }

    jmethodID getNameMethod = lc->env->GetMethodID(entityPlayerSPClass, "getName", "()Ljava/lang/String;");
    if (!getNameMethod) {
        std::cerr << "getName method not found." << std::endl;
        return "";
    }

    jstring name = (jstring)lc->env->CallObjectMethod(playerInstance, getNameMethod);
    if (lc->env->ExceptionOccurred()) {
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
        return "";
    }

    const char* nameChars = lc->env->GetStringUTFChars(name, nullptr);
    std::string playerName(nameChars);
    lc->env->ReleaseStringUTFChars(name, nameChars);
    lc->env->DeleteLocalRef(name);

    return playerName;
}
