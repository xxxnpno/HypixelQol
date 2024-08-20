#include "Minecraft.h"
#include <iostream>

jclass CMinecraft::GetClass() {
    jclass cls = lc->GetClass("net.minecraft.client.Minecraft");
    if (cls) {
        std::cout << "CMinecraft class obtained successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to obtain CMinecraft class." << std::endl;
    }
    return cls;
}

jobject CMinecraft::GetInstance() {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return nullptr;

    jfieldID getMinecraft = lc->env->GetStaticFieldID(minecraftClass, "theMinecraft", "Lnet/minecraft/client/Minecraft;");
    if (lc->env->ExceptionOccurred()) {
        std::cerr << "Exception occurred while getting field ID for theMinecraft." << std::endl;
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
        return nullptr;
    }

    jobject rtrn = lc->env->GetStaticObjectField(minecraftClass, getMinecraft);
    if (lc->env->ExceptionOccurred()) {
        std::cerr << "Exception occurred while getting static object field for theMinecraft." << std::endl;
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
    }
    else {
        std::cout << "Minecraft instance obtained successfully." << std::endl;
    }

    return rtrn;
}

CPlayer CMinecraft::GetLocalPlayer() {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return CPlayer(nullptr);

    jobject minecraftObject = GetInstance();
    if (!minecraftObject) return CPlayer(nullptr);

    jfieldID getPlayer = lc->env->GetFieldID(minecraftClass, "thePlayer", "Lnet/minecraft/client/entity/EntityPlayerSP;");
    if (lc->env->ExceptionOccurred()) {
        std::cerr << "Exception occurred while getting field ID for thePlayer." << std::endl;
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
        lc->env->DeleteLocalRef(minecraftObject);
        return CPlayer(nullptr);
    }

    jobject rtrn = lc->env->GetObjectField(minecraftObject, getPlayer);
    if (lc->env->ExceptionOccurred()) {
        std::cerr << "Exception occurred while getting object field for thePlayer." << std::endl;
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
    }
    else {
        std::cout << "Local player instance obtained successfully." << std::endl;
    }

    lc->env->DeleteLocalRef(minecraftObject);
    return CPlayer(rtrn);
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

    jmethodID addChatMessage = lc->env->GetMethodID(playerClass, "addChatComponentMessage", "(Lnet/minecraft/util/IChatComponent;)V");
    if (!addChatMessage) {
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

    lc->env->CallVoidMethod(localPlayer, addChatMessage, chatComponentInstance);
    if (lc->env->ExceptionOccurred()) {
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
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
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear();
    }

    lc->env->DeleteLocalRef(jmessage);
    lc->env->DeleteLocalRef(localPlayer);
    lc->env->DeleteLocalRef(minecraftInstance);
}



