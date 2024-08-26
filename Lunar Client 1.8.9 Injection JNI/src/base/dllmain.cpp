#include <Windows.h>
#include <iostream>
#include "../init.h"

FILE* file = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        AllocConsole();
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONIN$", "r", stdin);
        freopen_s(&file, "CONOUT$", "w", stderr);

        HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr);
        if (thread == nullptr) {
            std::cerr << "Failed to create thread." << std::endl;
            return FALSE;
        }
        CloseHandle(thread);
    }
    return TRUE;
}
