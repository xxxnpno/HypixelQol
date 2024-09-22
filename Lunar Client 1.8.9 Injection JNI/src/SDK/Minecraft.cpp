#include "Minecraft.h"
#include <iostream>

jclass CMinecraft::GetClass() {
    static jclass minecraftClass = lc->GetClass("net.minecraft.client.Minecraft");

    if (!minecraftClass) {
    }
    return minecraftClass;
}


jobject CMinecraft::GetInstance() {
    jclass minecraftClass = GetClass();
    if (!minecraftClass) return nullptr;

    jfieldID minecraftField = lc->env->GetStaticFieldID(minecraftClass, "theMinecraft", "Lnet/minecraft/client/Minecraft;");
    if (lc->env->ExceptionOccurred()) {
        return nullptr;
    }

    jobject minecraftInstance = lc->env->GetStaticObjectField(minecraftClass, minecraftField);
    if (lc->env->ExceptionOccurred()) {
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
        lc->env->DeleteLocalRef(minecraftInstance);
        return CPlayer(nullptr);
    }

    jobject playerInstance = lc->env->GetObjectField(minecraftInstance, playerField);

    lc->env->DeleteLocalRef(minecraftInstance);
    return CPlayer(playerInstance);
}

void CMinecraft::SendChatMessage(const std::string& message) {
    jclass chatComponentClass = lc->GetClass("net.minecraft.util.ChatComponentText");
    jmethodID chatComponentConstructor = lc->env->GetMethodID(chatComponentClass, "<init>", "(Ljava/lang/String;)V");
    jstring jmessage = lc->env->NewStringUTF(message.c_str());
    jobject chatComponentInstance = lc->env->NewObject(chatComponentClass, chatComponentConstructor, jmessage);

    jclass playerClass = lc->GetClass("net.minecraft.client.entity.EntityPlayerSP");
    jmethodID addChatMessageMethod = lc->env->GetMethodID(playerClass, "addChatComponentMessage", "(Lnet/minecraft/util/IChatComponent;)V");
    jobject localPlayer = GetLocalPlayer().GetPlayerInstance();

    lc->env->CallVoidMethod(localPlayer, addChatMessageMethod, chatComponentInstance);

    if (lc->env->ExceptionCheck()) {
        lc->env->ExceptionDescribe();
        lc->env->ExceptionClear(); 
    }

    lc->env->DeleteLocalRef(jmessage); 
    lc->env->DeleteLocalRef(chatComponentInstance); 
    lc->env->DeleteLocalRef(localPlayer); 
}



void CMinecraft::SendPlayerChatMessage(const std::string& message) {
    jclass minecraftClass = GetClass();
    jobject minecraftInstance = GetInstance();
    jclass playerClass = lc->GetClass("net.minecraft.client.entity.EntityPlayerSP");
    jmethodID sendChatMessageMethod = lc->env->GetMethodID(playerClass, "sendChatMessage", "(Ljava/lang/String;)V");
    jobject localPlayer = GetLocalPlayer().GetPlayerInstance();
    jstring jmessage = lc->env->NewStringUTF(message.c_str());

    lc->env->CallVoidMethod(localPlayer, sendChatMessageMethod, jmessage);

    lc->env->DeleteLocalRef(jmessage);
    lc->env->DeleteLocalRef(localPlayer);
    lc->env->DeleteLocalRef(minecraftInstance);
}
