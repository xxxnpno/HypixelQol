#include "Player.h"

CPlayer::CPlayer(jobject instance) : playerInstance(instance) {
}

void CPlayer::Cleanup() {
    if (playerInstance) {
        lc->env->DeleteLocalRef(playerInstance);
    }
    else {
        std::cerr << "CPlayer instance is null, nothing to clean up." << std::endl;
    }
}

jclass CPlayer::GetClass() {
    jclass cls = lc->GetClass("net.minecraft.entity.Entity");
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

std::string CPlayer::GetPlayerBrand() {
    jclass clientBrandRetrieverClass = lc->GetClass("net.minecraft.client.ClientBrandRetriever");
    if (!clientBrandRetrieverClass) {
        std::cerr << "ClientBrandRetriever class not found." << std::endl;
        return "";
    }

    jmethodID getClientModNameMethod = lc->env->GetStaticMethodID(clientBrandRetrieverClass, "getClientModName", "()Ljava/lang/String;");
    if (!getClientModNameMethod) {
        std::cerr << "getClientModName method not found." << std::endl;
        return "";
    }

    jstring brand = (jstring)lc->env->CallStaticObjectMethod(clientBrandRetrieverClass, getClientModNameMethod);
    if (lc->env->ExceptionOccurred()) {
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
        return "";
    }

    const char* brandChars = lc->env->GetStringUTFChars(brand, nullptr);
    std::string playerBrand(brandChars);
    lc->env->ReleaseStringUTFChars(brand, brandChars);
    lc->env->DeleteLocalRef(brand);

    return playerBrand;
}



bool CPlayer::isSneaking()
{
    jmethodID isSneaking = lc->env->GetMethodID(this->GetClass(), "isSneaking", "()Z");

    bool rtrn = lc->env->CallBooleanMethod(this->playerInstance, isSneaking);

    return rtrn;
}

bool CPlayer::isSprinting()
{
    jmethodID isSprinting = lc->env->GetMethodID(this->GetClass(), "isSprinting", "()Z");

    bool rtrn = lc->env->CallBooleanMethod(this->playerInstance, isSprinting);

    return rtrn;
}

float CPlayer::rotationYaw()
{
    jfieldID rotationYaw = lc->env->GetFieldID(this->GetClass(), "rotationYaw", "F");

    float rtrn = lc->env->GetFloatField(this->playerInstance, rotationYaw);

    return rtrn;

}

float CPlayer::rotationPitch()
{
    jfieldID rotationPitch = lc->env->GetFieldID(this->GetClass(), "rotationPitch", "F");

    float rtrn = lc->env->GetFloatField(this->playerInstance, rotationPitch);

    return rtrn;

}

void CPlayer::SetCursor(float p, float y)
{
    jfieldID rotationPitch = lc->env->GetFieldID(this->GetClass(), "rotationPitch", "F");
    jfieldID rotationYaw = lc->env->GetFieldID(this->GetClass(), "rotationYaw", "F");

    lc->env->SetDoubleField(this->playerInstance, rotationPitch, p);
    lc->env->SetDoubleField(this->playerInstance, rotationYaw, y);
}

void CPlayer::setSprinting(bool s) {

    jmethodID setSprinting = lc->env->GetMethodID(this->GetClass(), "setSprinting", "(Z)V");

    lc->env->CallVoidMethod(this->playerInstance, setSprinting, s);

}

void CPlayer::setSneaking(bool n) {

    jmethodID setSneaking = lc->env->GetMethodID(this->GetClass(), "setSneaking", "(Z)V");

    lc->env->CallVoidMethod(this->playerInstance, setSneaking, n);

}

