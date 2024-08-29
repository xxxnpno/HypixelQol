#include <Windows.h>
#include "../init.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr);
        if (thread == nullptr) {
            return FALSE;
        }
        CloseHandle(thread);
    }
    return TRUE;
}
