#pragma once
#include<windows.h>
#include<tlhelp32.h>
#include <iostream>
DWORD GetModuleBaseAddress(DWORD dwProcessId, const wchar_t* szModuleName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcessId);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create process snapshot." << std::endl;
        return 0;
    }

    MODULEENTRY32 moduleEntry = { sizeof(MODULEENTRY32) };
    BOOL bModuleFound = Module32First(hSnapshot, &moduleEntry);
    while (bModuleFound)
    {
        if (_wcsicmp(moduleEntry.szModule, szModuleName) == 0)
        {
            CloseHandle(hSnapshot);
            return reinterpret_cast<DWORD>(moduleEntry.modBaseAddr);
        }

        bModuleFound = Module32Next(hSnapshot, &moduleEntry);
    }

    CloseHandle(hSnapshot);
    return 0;
}
