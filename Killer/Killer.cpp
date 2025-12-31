#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <locale.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

using namespace std;

wstring GetProcessName(DWORD pid);
BOOL KillProcessById(DWORD pid);
BOOL KillUwpCalculator();
BOOL KillProcessesByNameRegular(const wchar_t* processName);
BOOL KillProcessesByName(const wchar_t* processName);
BOOL KillProcessesFromEnvironment();
void PrintHelp();


wstring GetProcessName(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess != NULL) {
        wchar_t buffer[MAX_PATH] = { 0 };
        if (GetProcessImageFileNameW(hProcess, buffer, MAX_PATH) > 0) {
            wchar_t* fileName = wcsrchr(buffer, L'\\');
            if (fileName != NULL) {
                fileName++;
                CloseHandle(hProcess);
                return wstring(fileName);
            }
        }
        CloseHandle(hProcess);
    }
    return L"";
}

BOOL KillProcessById(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        DWORD err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            printf("   Нет прав для завершения процесса %d (требуется администратор)\n", pid);

            wstring procName = GetProcessName(pid);
            if (!procName.empty()) {
                wprintf(L"   Имя процесса: %s\n", procName.c_str());

                if (procName.find(L"Calculator") != wstring::npos ||
                    procName.find(L"calc") != wstring::npos) {
                    printf("   Пробуем taskkill...\n");

                    char cmd[256];
                    sprintf_s(cmd, sizeof(cmd), "taskkill /PID %d /F", pid);
                    if (system(cmd) == 0) {
                        printf("   Процесс завершен через taskkill\n");
                        return TRUE;
                    }
                }
            }
        }
        printf("Ошибка открытия процесса %d. Код: %d\n", pid, err);
        return FALSE;
    }

    if (TerminateProcess(hProcess, 0)) {
        printf("   Процесс %d успешно завершен\n", pid);
        CloseHandle(hProcess);
        return TRUE;
    }
    else {
        DWORD err = GetLastError();
        printf("Ошибка завершения процесса %d. Код: %d\n", pid, err);

        char cmd[256];
        sprintf_s(cmd, sizeof(cmd), "taskkill /PID %d /F", pid);
        if (system(cmd) == 0) {
            printf("   Процесс завершен через taskkill\n");
            CloseHandle(hProcess);
            return TRUE;
        }

        CloseHandle(hProcess);
        return FALSE;
    }
}


BOOL KillProcessesByNameRegular(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Ошибка создания снимка процессов. Код: %d\n", GetLastError());
        return FALSE;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    BOOL found = FALSE;
    DWORD currentPid = GetCurrentProcessId();

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                wprintf(L"   Найден: %s (PID: %d)\n", pe.szExeFile, pe.th32ProcessID);

                if (pe.th32ProcessID == currentPid) {
                    printf("   Пропускаем себя\n");
                    continue;
                }

                if (KillProcessById(pe.th32ProcessID)) {
                    found = TRUE;
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    if (!found) {
        wprintf(L"Процессы '%s' не найдены\n", processName);
    }

    return found;
}

BOOL KillProcessesByName(const wchar_t* processName) {
    printf("Поиск процессов: ");
    wprintf(L"'%s'\n", processName);
    return KillProcessesByNameRegular(processName);
}

BOOL KillProcessesFromEnvironment() {
    char envVar[4096];
    DWORD len = GetEnvironmentVariableA("PROC_TO_KILL", envVar, sizeof(envVar));

    if (len == 0) {
        printf("Переменная PROC_TO_KILL не найдена\n");
        return FALSE;
    }

    printf("Переменная PROC_TO_KILL: %s\n", envVar);

    char* token;
    char* next_token = NULL;
    BOOL killedAny = FALSE;

    token = strtok_s(envVar, ",", &next_token);
    while (token != NULL) {
        char* cleaned = token;
        while (*cleaned == ' ' || *cleaned == '\"') cleaned++;

        char* end = cleaned + strlen(cleaned) - 1;
        while (end > cleaned && (*end == ' ' || *end == '\"')) {
            *end = '\0';
            end--;
        }

        if (strlen(cleaned) > 0) {
            printf("Завершение: %s\n", cleaned);

            wstring wname;
            int size = MultiByteToWideChar(CP_ACP, 0, cleaned, -1, NULL, 0);
            if (size > 0) {
                wname.resize(size - 1);
                MultiByteToWideChar(CP_ACP, 0, cleaned, -1, &wname[0], size);

                if (KillProcessesByName(wname.c_str())) {
                    killedAny = TRUE;
                }
            }
        }

        token = strtok_s(NULL, ",", &next_token);
    }

    return killedAny;
}

void PrintHelp() {
    printf("=== Killer ===\n");
    printf("Использование:\n");
    printf("  Killer.exe --id <PID>\n");
    printf("  Killer.exe --name <имя>\n");
    printf("  Killer.exe\n");
    printf("  Killer.exe --help\n");
    printf("\nПримеры:\n");
    printf("  Killer.exe --id 1234\n");
    printf("  Killer.exe --name notepad.exe\n");
    printf("  Killer.exe --name Calculator.exe\n");
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    printf("=== Killer ===\n\n");

    if (argc == 1) {
        return KillProcessesFromEnvironment() ? 0 : 1;
    }
    else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        PrintHelp();
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1], "--id") == 0) {
        DWORD pid = atoi(argv[2]);
        if (pid == 0) {
            printf("Неверный PID\n");
            return 1;
        }

        if (pid == GetCurrentProcessId()) {
            printf("Нельзя завершить себя\n");
            return 1;
        }

        return KillProcessById(pid) ? 0 : 1;
    }
    else if (argc == 3 && strcmp(argv[1], "--name") == 0) {
        wstring wname;
        int size = MultiByteToWideChar(CP_ACP, 0, argv[2], -1, NULL, 0);
        if (size > 0) {
            wname.resize(size - 1);
            MultiByteToWideChar(CP_ACP, 0, argv[2], -1, &wname[0], size);

            return KillProcessesByName(wname.c_str()) ? 0 : 1;
        }
        return 1;
    }
    else {
        printf("Неверные параметры\n");
        PrintHelp();
        return 1;
    }

    return 0;
}