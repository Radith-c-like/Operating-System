#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void CreateChildProcess(const char* name, HANDLE hIn, HANDLE hOut, PROCESS_INFORMATION& pi) {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (!CreateProcessA(name, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Ошибка запуска " << name << ": " << GetLastError() << std::endl;
        exit(1);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    HANDLE hPipeMainM[2], hPipeMA[2], hPipeAP[2], hPipePS[2], hPipeSMain[2];
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    CreatePipe(&hPipeMainM[0], &hPipeMainM[1], &sa, 0);
    CreatePipe(&hPipeMA[0], &hPipeMA[1], &sa, 0);
    CreatePipe(&hPipeAP[0], &hPipeAP[1], &sa, 0);
    CreatePipe(&hPipePS[0], &hPipePS[1], &sa, 0);
    CreatePipe(&hPipeSMain[0], &hPipeSMain[1], &sa, 0);

    PROCESS_INFORMATION piM, piA, piP, piS;
    CreateChildProcess("ProcessM.exe", hPipeMainM[0], hPipeMA[1], piM);
    CreateChildProcess("ProcessA.exe", hPipeMA[0], hPipeAP[1], piA);
    CreateChildProcess("ProcessP.exe", hPipeAP[0], hPipePS[1], piP);
    CreateChildProcess("ProcessS.exe", hPipePS[0], hPipeSMain[1], piS);

    CloseHandle(hPipeMainM[0]);
    CloseHandle(hPipeMA[0]); CloseHandle(hPipeMA[1]);
    CloseHandle(hPipeAP[0]); CloseHandle(hPipeAP[1]);
    CloseHandle(hPipePS[0]); CloseHandle(hPipePS[1]);
    CloseHandle(hPipeSMain[1]);

    std::string input;
    std::cout << "Введите числа через пробел: ";
    std::getline(std::cin, input);
    input += "\n";

    DWORD written;
    WriteFile(hPipeMainM[1], input.c_str(), (DWORD)input.length(), &written, NULL);
    CloseHandle(hPipeMainM[1]);

    char buffer[128];
    DWORD read;
    std::string result;
    if (ReadFile(hPipeSMain[0], buffer, sizeof(buffer) - 1, &read, NULL)) {
        buffer[read] = '\0';
        result = buffer;
    }

    std::cout << "Результат из цепочки: " << result;

    WaitForSingleObject(piS.hProcess, INFINITE);
    CloseHandle(piM.hProcess); CloseHandle(piM.hThread);
    CloseHandle(piA.hProcess); CloseHandle(piA.hThread);
    CloseHandle(piP.hProcess); CloseHandle(piP.hThread);
    CloseHandle(piS.hProcess); CloseHandle(piS.hThread);
    CloseHandle(hPipeSMain[0]);

    std::cout << "\nНажмите Enter, чтобы выйти...";
    std::cin.clear();
    std::cin.ignore(1000, '\n');
    std::cin.get();

    return 0;
}