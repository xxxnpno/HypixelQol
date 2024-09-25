/*#include <Windows.h>
#include <iostream>
#include "../init.h"

FILE* fileOut = nullptr; 
FILE* fileErr = nullptr; 

void CleanupConsole() {
    if (fileOut) {
        fclose(fileOut);
        fileOut = nullptr;
    }
    if (fileErr) {
        fclose(fileErr);
        fileErr = nullptr;
    }
    FreeConsole();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        if (AllocConsole()) {
            if (freopen_s(&fileOut, "CONOUT$", "w", stdout) != 0) {
                return FALSE;
            }
            if (freopen_s(&fileErr, "CONOUT$", "w", stderr) != 0) {
                return FALSE;
            }

            HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr);
            if (thread == nullptr) {
                CleanupConsole();
                return FALSE;
            }
            CloseHandle(thread);
        }
        else {
            return FALSE;
        }
        break;

    case DLL_PROCESS_DETACH:
        CleanupConsole();
        break;
    }
    return TRUE;
}*/

#include <Windows.h>
#include "../init.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(init), hModule, 0, nullptr);
        if (thread == nullptr) {
            return FALSE;
        }
        CloseHandle(thread);
        break;
    }

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
