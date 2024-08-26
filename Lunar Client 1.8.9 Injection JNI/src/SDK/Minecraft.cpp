#include "Minecraft.h"
#include <iostream>

jclass CMinecraft::GetClass() {
    static jclass minecraftClass = lc->GetClass("net.minecraft.client.Minecraft");

    if (!minecraftClass) {
        std::cerr << "Failed to obtain CMinecraft class." << std::endl;
    }
    return minecraftClass;
}

void HandleException(const std::string& errorMessage) {
    std::cerr << errorMessage << std::endl;
    lc->env->ExceptionDescribe();
    lc->env->ExceptionClear();
}

jobject CMinecraft::GetInstance() {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return nullptr;

    jfieldID minecraftField = lc->env->GetStaticFieldID(minecraftClass, "theMinecraft", "Lnet/minecraft/client/Minecraft;");
    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while getting field ID for theMinecraft.");
        return nullptr;
    }

    jobject minecraftInstance = lc->env->GetStaticObjectField(minecraftClass, minecraftField);
    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while getting static object field for theMinecraft.");
        return nullptr;
    }

    return minecraftInstance;
}

CPlayer CMinecraft::GetLocalPlayer() {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return CPlayer(nullptr);

    jobject minecraftInstance = GetInstance();
    if (!minecraftInstance) return CPlayer(nullptr);

    jfieldID playerField = lc->env->GetFieldID(minecraftClass, "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while getting field ID for thePlayer.");
        lc->env->DeleteLocalRef(minecraftInstance);
        return CPlayer(nullptr);
    }

    jobject playerInstance = lc->env->GetObjectField(minecraftInstance, playerField);
    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while getting object field for thePlayer.");
    }


    lc->env->DeleteLocalRef(minecraftInstance);
    return CPlayer(playerInstance);
}

void CMinecraft::SendChatMessage(const std::string& message) {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return;

    jobject minecraftInstance = GetInstance();
    if (!minecraftInstance) return;

    jclass chatComponentClass = lc->GetClass("net.minecraft.util.ChatComponentText");
    if (!chatComponentClass) {
        std::cerr << "ChatComponentText class not found." << std::endl;
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jmethodID chatComponentConstructor = lc->env->GetMethodID(chatComponentClass, "<init>", "(Ljava/lang/String;)V");
    if (!chatComponentConstructor) {
        std::cerr << "ChatComponentText constructor not found." << std::endl;
        lc->env->DeleteLocalRef(chatComponentClass);
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jstring jmessage = lc->env->NewStringUTF(message.c_str());
    jobject chatComponentInstance = lc->env->NewObject(chatComponentClass, chatComponentConstructor, jmessage);
    if (!chatComponentInstance) {
        std::cerr << "ChatComponentText instance not created." << std::endl;
        lc->env->DeleteLocalRef(jmessage);
        lc->env->DeleteLocalRef(chatComponentClass);
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jclass playerClass = lc->GetClass("net.minecraft.client.entity.EntityPlayerSP");
    if (!playerClass) {
        std::cerr << "EntityPlayerSP class not found." << std::endl;
        lc->env->DeleteLocalRef(chatComponentInstance);
        lc->env->DeleteLocalRef(jmessage);
        lc->env->DeleteLocalRef(chatComponentClass);
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jmethodID addChatMessageMethod = lc->env->GetMethodID(playerClass, "addChatComponentMessage", "(Lnet/minecraft/util/IChatComponent;)V");
    if (!addChatMessageMethod) {
        std::cerr << "addChatComponentMessage method not found." << std::endl;
        lc->env->DeleteLocalRef(chatComponentInstance);
        lc->env->DeleteLocalRef(jmessage);
        lc->env->DeleteLocalRef(chatComponentClass);
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jobject localPlayer = GetLocalPlayer().GetPlayerInstance();
    if (!localPlayer) {
        std::cerr << "Local player instance not found." << std::endl;
        lc->env->DeleteLocalRef(chatComponentInstance);
        lc->env->DeleteLocalRef(jmessage);
        lc->env->DeleteLocalRef(chatComponentClass);
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    lc->env->CallVoidMethod(localPlayer, addChatMessageMethod, chatComponentInstance);
    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while calling addChatComponentMessage.");
    }

    lc->env->DeleteLocalRef(jmessage);
    lc->env->DeleteLocalRef(chatComponentInstance);
    lc->env->DeleteLocalRef(chatComponentClass);
    lc->env->DeleteLocalRef(minecraftInstance);
    lc->env->DeleteLocalRef(localPlayer);
}

void CMinecraft::SendPlayerChatMessage(const std::string& message) {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return;

    jobject minecraftInstance = GetInstance();
    if (!minecraftInstance) return;

    jclass playerClass = lc->GetClass("net.minecraft.client.entity.EntityPlayerSP");
    if (!playerClass) {
        std::cerr << "EntityPlayerSP class not found." << std::endl;
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jmethodID sendChatMessageMethod = lc->env->GetMethodID(playerClass, "sendChatMessage", "(Ljava/lang/String;)V");
    if (!sendChatMessageMethod) {
        std::cerr << "sendChatMessage method not found." << std::endl;
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jobject localPlayer = GetLocalPlayer().GetPlayerInstance();
    if (!localPlayer) {
        std::cerr << "Local player instance not found." << std::endl;
        lc->env->DeleteLocalRef(minecraftInstance);
        return;
    }

    jstring jmessage = lc->env->NewStringUTF(message.c_str());
    lc->env->CallVoidMethod(localPlayer, sendChatMessageMethod, jmessage);

    if (lc->env->ExceptionOccurred()) {
        HandleException("Exception occurred while calling sendChatMessage.");
    }

    lc->env->DeleteLocalRef(jmessage);
    lc->env->DeleteLocalRef(localPlayer);
    lc->env->DeleteLocalRef(minecraftInstance);
}