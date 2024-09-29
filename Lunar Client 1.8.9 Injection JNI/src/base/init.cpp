#include "../init.h"

auto Minecraft = std::make_unique<CMinecraft>();

void init(void* instance) {
    std::cout << "Initializing..." << std::endl;

    jsize count;
    if (JNI_GetCreatedJavaVMs(&lc->vm, 1, &count) != JNI_OK || count == 0) {
        std::cerr << "Failed to get created Java VMs, exiting." << std::endl;
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
        return;
    }

    std::cout << "Java VM count: " << count << std::endl;

    if (lc->vm->GetEnv(reinterpret_cast<void**>(&lc->env), JNI_VERSION_1_8) == JNI_EDETACHED) {
        std::cout << "Attaching current thread to JVM..." << std::endl;
        if (lc->vm->AttachCurrentThread(reinterpret_cast<void**>(&lc->env), nullptr) != JNI_OK) {
            std::cerr << "Failed to attach current thread, exiting." << std::endl;
            FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
            return;
        }
    }

    if (lc->env) {
        std::cout << "Environment successfully obtained." << std::endl;

        lc->GetLoadedClasses();

        std::string logFilePath = getUserPath() + "\\.lunarclient\\offline\\multiver\\logs\\latest.log";
        std::cout << "Log file path: " << logFilePath << std::endl;

        std::ofstream logFile(logFilePath, std::ios::out | std::ios::trunc);
        std::cout << "Log file opened for writing." << std::endl;

        std::string r = performRequest("https://kerhosting.com/name-apikey.json");
        std::cout << "Received response from API: " << r << std::endl;

        bool s = extractPlayer(r);
        if (s) {
            std::string rankDisplay = rankUtils("https://api.hypixel.net/player?key=" + getApiKey() + "&name=" + Minecraft->GetLocalPlayer().GetName());
            std::cout << "Player rank: " << rankDisplay << std::endl;

            Minecraft->SendChatMessage("§eWelcome" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "                                   ");
            //Minecraft->SendTitleMessage("§eWelcome" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "     ", "", 10, 70, 20);

            while (!GetAsyncKeyState(VK_DELETE)) {
                readLogFile(logFilePath);
                Sleep(20);
            }

            //Minecraft->SendTitleMessage("§eGoodbye" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "     ", "", 10, 70, 20);
            Minecraft->SendChatMessage("§eGoodbye" + rankDisplay + " " + Minecraft->GetLocalPlayer().GetName() + "                                   ");
        }
        else {
            std::cerr << "Failed to extract player data." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to get JVM environment, exiting." << std::endl;
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 1);
    }

    lc->vm->DetachCurrentThread();
    std::cout << "Detached current thread from JVM." << std::endl;
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}