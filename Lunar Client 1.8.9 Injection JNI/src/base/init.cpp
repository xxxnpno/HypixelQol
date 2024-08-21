#include "../init.h"

extern FILE* file;

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
        std::cerr << "Failed to get Java VM." << std::endl;
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
        return;
    }

    std::cout << "Java VM obtained successfully." << std::endl;

    if (lc->vm->GetEnv(reinterpret_cast<void**>(&lc->env), JNI_VERSION_1_8) == JNI_EDETACHED) {
        if (lc->vm->AttachCurrentThread(reinterpret_cast<void**>(&lc->env), nullptr) != JNI_OK) {
            std::cerr << "Failed to attach to Java VM." << std::endl;
            FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
            return;
        }
    }

    if (lc->env) {
        lc->GetLoadedClasses();
        std::cout << "DLL successfully attached." << std::endl;

        CPlayer localPlayer = Minecraft->GetLocalPlayer();
        std::string playerName = localPlayer.GetName();

        Minecraft->SendChatMessage("§aYou are playing on : " + playerName + "                                    ");

        std::string userPath = getUserPath();
        std::string logFilePath = userPath + "\\.lunarclient\\offline\\multiver\\logs\\latest.log";

        {
            std::ofstream logFile(logFilePath, std::ios::out | std::ios::trunc);
            if (!logFile) {
                std::cerr << "Failed to open log file for truncation." << std::endl;
            }
            else {
                std::cout << "Log file truncated successfully." << std::endl;
            }
        }

        while (!GetAsyncKeyState(VK_DELETE)) {
            readLogFile(logFilePath);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    else {
        std::cerr << "Failed to attach to Java VM." << std::endl;
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
    }

    std::cout << "DLL successfully detached." << std::endl;
    fclose(file);
    FreeConsole();
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    lc->vm->DetachCurrentThread();
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}
