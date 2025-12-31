#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

struct ProcessInfo {
    wstring name;
    DWORD pid;
    BOOL isRunning;
};

BOOL IsProcessRunning(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return TRUE;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return FALSE;
}

BOOL IsProcessRunningById(DWORD pid) {
    if (pid == 0) return FALSE;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        return FALSE;
    }

    DWORD exitCode;
    if (GetExitCodeProcess(hProcess, &exitCode)) {
        CloseHandle(hProcess);
        return (exitCode == STILL_ACTIVE);
    }

    CloseHandle(hProcess);
    return FALSE;
}

BOOL RunKiller(const char* parameters) {
    printf("   Запуск: Killer.exe %s\n", parameters);

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    char command[MAX_PATH * 2];
    sprintf_s(command, sizeof(command), "\"%s\\Killer.exe\" %s", currentDir, parameters);

    int result = system(command);

    if (result == 0) {
        printf("   Killer выполнился успешно\n");
        return TRUE;
    }
    else {
        printf("   Ошибка Killer. Код: %d\n", result);
        return FALSE;
    }
}

ProcessInfo StartProcess(const wchar_t* processName) {
    ProcessInfo info;
    info.name = processName;
    info.pid = 0;
    info.isRunning = FALSE;

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    wchar_t commandLine[MAX_PATH];
    wcscpy_s(commandLine, MAX_PATH, processName);

    if (CreateProcessW(NULL, commandLine, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        info.pid = pi.dwProcessId;
        info.isRunning = TRUE;

        wprintf(L"    Запущен: %s (PID: %d)\n", processName, info.pid);

        Sleep(1500);

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    else {
        wprintf(L"   Ошибка запуска %s. Код: %d\n", processName, GetLastError());
    }

    return info;
}

DWORD FindProcessPid(const wchar_t* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName) == 0) {
                DWORD pid = pe.th32ProcessID;
                CloseHandle(hSnapshot);
                return pid;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

void DemonstrateKilling(const wchar_t* processName, const char* testName) {
    wprintf(L"\n=== %s: %s ===\n", testName, processName);

    if (!IsProcessRunning(processName)) {
        printf("Процесс не запущен, запускаем...\n");
        ProcessInfo info = StartProcess(processName);
        if (!info.isRunning) {
            printf("Не удалось запустить процесс для демонстрации\n");
            return;
        }
        Sleep(2000);
    }

    BOOL before = IsProcessRunning(processName);
    DWORD pid = FindProcessPid(processName);

    if (before && pid > 0) {
        wprintf(L"До: %s запущен (PID: %d)\n", processName, pid);

        char command[256];
        if (wcscmp(processName, L"calc.exe") == 0) {
            sprintf_s(command, sizeof(command), "--name Calculator.exe");
        }
        else {
            sprintf_s(command, sizeof(command), "--name %S", processName);
        }

        RunKiller(command);

        Sleep(2000);
        BOOL after = IsProcessRunning(processName);

        wprintf(L"После: %s запущен? %s\n", processName, after ? L"ДА" : L"НЕТ");

        if (before && !after) {
            printf("   УСПЕХ: Процесс завершен!\n");
        }
        else if (after) {
            printf("   Процесс все еще запущен\n");
        }
    }
    else {
        wprintf(L"Процесс %s не найден\n", processName);
    }
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    printf("========================================\n");
    printf("    User App - Демонстрация Killer\n");
    printf("========================================\n\n");

    char killerPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, killerPath);
    strcat_s(killerPath, "\\Killer.exe");

    if (GetFileAttributesA(killerPath) == INVALID_FILE_ATTRIBUTES) {
        printf("ОШИБКА: Killer.exe не найден!\n");
        printf("Поместите Killer.exe в папку: %s\n", killerPath);
        printf("\nНажмите Enter для выхода...\n");
        getchar();
        return 1;
    }

    printf(" Killer.exe найден\n\n");

    printf("1. УСТАНОВКА ПЕРЕМЕННОЙ ОКРУЖЕНИЯ\n");
    printf("----------------------------------------\n");

    const char* procList = "\"notepad.exe,calc.exe,mspaint.exe\"";
    if (!SetEnvironmentVariableA("PROC_TO_KILL", procList)) {
        printf("Ошибка установки переменной. Код: %d\n", GetLastError());
    }
    else {
        printf("Установлена переменная: PROC_TO_KILL = %s\n", procList);

        char checkVar[256];
        if (GetEnvironmentVariableA("PROC_TO_KILL", checkVar, sizeof(checkVar)) > 0) {
            printf(" Переменная успешно установлена: %s\n", checkVar);
        }
    }

    printf("\n2. ЗАПУСК ТЕСТОВЫХ ПРОЦЕССОВ\n");
    printf("----------------------------------------\n");

    vector<ProcessInfo> processes;

    printf("a) Блокнот (notepad.exe):\n");
    processes.push_back(StartProcess(L"notepad.exe"));

    printf("\nb) Калькулятор (calc.exe):\n");
    system("start calc");
    Sleep(3000);
    DWORD calcPid = FindProcessPid(L"Calculator.exe");
    if (calcPid == 0) calcPid = FindProcessPid(L"calc.exe");
    if (calcPid > 0) {
        wprintf(L"   Найден калькулятор с PID: %d\n", calcPid);
        ProcessInfo calcInfo;
        calcInfo.name = L"Calculator.exe";
        calcInfo.pid = calcPid;
        calcInfo.isRunning = TRUE;
        processes.push_back(calcInfo);
    }

    printf("\nc) Paint (mspaint.exe):\n");
    processes.push_back(StartProcess(L"mspaint.exe"));

    Sleep(2000);

    printf("\n3. ДЕМОНСТРАЦИЯ: ЗАВЕРШЕНИЕ ПО ID\n");
    printf("----------------------------------------\n");

    for (auto& proc : processes) {
        if (proc.name == L"notepad.exe" && proc.pid > 0) {
            printf("Тестируем завершение Блокнота по ID:\n");
            wprintf(L"   Процесс: %s (PID: %d)\n", proc.name.c_str(), proc.pid);

            BOOL before = IsProcessRunningById(proc.pid);
            printf("   До: процесс запущен? %s\n", before ? "ДА" : "НЕТ");

            char cmd[100];
            sprintf_s(cmd, sizeof(cmd), "--id %d", proc.pid);
            RunKiller(cmd);

            Sleep(1500);
            BOOL after = IsProcessRunningById(proc.pid);
            printf("   После: процесс запущен? %s\n", after ? "ДА" : "НЕТ");

            if (before && !after) {
                printf("   Блокнот успешно завершен по ID!\n");
                proc.isRunning = FALSE;
            }
            break;
        }
    }

    printf("\n4. ДЕМОНСТРАЦИЯ: ЗАВЕРШЕНИЕ ПО ИМЕНИ\n");
    printf("----------------------------------------\n");

    DemonstrateKilling(L"mspaint.exe", "Paint");

    DemonstrateKilling(L"CalculatorApp.exe", "Калькулятор");

    printf("\n5. ДЕМОНСТРАЦИЯ: ЧЕРЕЗ ПЕРЕМЕННУЮ ОКРУЖЕНИЯ\n");
    printf("----------------------------------------\n");

    printf("Запускаем процессы заново для демонстрации...\n");

    vector<ProcessInfo> newProcesses;
    newProcesses.push_back(StartProcess(L"notepad.exe"));

    system("start calc");
    Sleep(3000);

    newProcesses.push_back(StartProcess(L"mspaint.exe"));

    Sleep(2000);

    printf("\nПроверяем перед использованием переменной окружения:\n");
    printf("   - Notepad.exe: %s\n", IsProcessRunning(L"notepad.exe") ? "запущен" : "не запущен");
    printf("   - Calc.exe: %s\n", IsProcessRunning(L"calc.exe") ? "запущен" : "не запущен");
    printf("   - MSPaint.exe: %s\n", IsProcessRunning(L"mspaint.exe") ? "запущен" : "не запущен");

    printf("\nЗапускаем Killer без параметров (будет использовать PROC_TO_KILL):\n");
    RunKiller("");

    printf("\nПроверяем после использования переменной окружения:\n");
    Sleep(2000);
    printf("   - Notepad.exe: %s\n", IsProcessRunning(L"notepad.exe") ? "запущен" : "не запущен");
    printf("   - Calc.exe: %s\n", IsProcessRunning(L"calc.exe") ? "запущен" : "не запущен");
    printf("   - MSPaint.exe: %s\n", IsProcessRunning(L"mspaint.exe") ? "запущен" : "не запущен");

    printf("\n6. УДАЛЕНИЕ ПЕРЕМЕННОЙ ОКРУЖЕНИЯ\n");
    printf("----------------------------------------\n");

    if (SetEnvironmentVariableA("PROC_TO_KILL", NULL)) {
        printf(" Переменная PROC_TO_KILL удалена\n");

        // Проверяем удаление
        char checkVar[256];
        if (GetEnvironmentVariableA("PROC_TO_KILL", checkVar, sizeof(checkVar)) == 0) {
            printf(" Переменная больше не существует\n");
        }
    }
    else {
        printf(" Ошибка удаления переменной\n");
    }

    printf("\n7. ФИНАЛЬНЫЙ ОТЧЕТ\n");
    printf("----------------------------------------\n");

    printf("Состояние процессов в системе:\n");
    printf("   • Notepad.exe: %s\n", IsProcessRunning(L"notepad.exe") ? " запущен" : " не запущен");
    printf("   • Calc.exe: %s\n", IsProcessRunning(L"calc.exe") ? " запущен" : " не запущен");
    printf("   • Calculator.exe: %s\n", IsProcessRunning(L"Calculator.exe") ? " запущен" : " не запущен");
    printf("   • MSPaint.exe: %s\n", IsProcessRunning(L"mspaint.exe") ? " запущен" : " не запущен");

    printf("\n========================================\n");
    printf("    Демонстрация завершена успешно!\n");
    printf("========================================\n\n");

    printf("\nНажмите Enter для выхода...\n");
    getchar();

    return 0;
}