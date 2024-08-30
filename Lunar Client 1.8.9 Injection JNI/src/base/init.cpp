#include "../init.h"

auto Minecraft = std::make_unique<CMinecraft>();

std::string getUserPath() {
    char userPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath))) {
        return std::string(userPath);
    }
    return "";
}

void init(void* instance) {
    jsize count;
    if (JNI_GetCreatedJavaVMs(&lc->vm, 1, &count) != JNI_OK || count == 0) {
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
        return;
    }

    if (lc->vm->GetEnv(reinterpret_cast<void**>(&lc->env), JNI_VERSION_1_8) == JNI_EDETACHED) {
        if (lc->vm->AttachCurrentThread(reinterpret_cast<void**>(&lc->env), nullptr) != JNI_OK) {
            FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
            return;
        }
    }

    if (lc->env) {
        lc->GetLoadedClasses();

        Minecraft->SendChatMessage("§aWelcome : " + Minecraft->GetLocalPlayer().GetName() + ", you are playing on : " + Minecraft->GetLocalPlayer().GetPlayerBrand() + "                                   ");
        
        std::string userPath = getUserPath();
        std::string logFilePath = userPath + "\\.lunarclient\\offline\\multiver\\logs\\latest.log";

        {
            std::ofstream logFile(logFilePath, std::ios::out | std::ios::trunc);
        }
        while (!GetAsyncKeyState(VK_DELETE)) {
            readLogFile(logFilePath);
            Sleep(20);
        }

    }
    else {
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
    }

    lc->vm->DetachCurrentThread();
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}
